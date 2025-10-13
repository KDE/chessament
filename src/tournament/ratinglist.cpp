// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglist.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QDir>
#include <QNetworkAccessManager>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QtConcurrent>

#include <KLocalizedString>
#include <KZip>

#include "db.h"

using namespace Qt::StringLiterals;

std::expected<QSqlDatabase, QString> RatingList::getDB(const QString &connectionName)
{
    if (QSqlDatabase::contains(connectionName)) {
        return QSqlDatabase::database(connectionName);
    }

    qDebug() << "opening ratings db";
    const QString dbFolder = QStandardPaths::writableLocation(QStandardPaths::StateLocation);
    QDir().mkpath(dbFolder);

    auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, connectionName);
    db.setDatabaseName(dbFolder + "/rating-lists.db"_L1);

    if (!db.open()) {
        return std::unexpected(db.lastError().text());
    }

    QSqlQuery query(db);
    query.prepare(ENABLE_FOREIGN_KEYS_QUERY);
    if (!query.exec()) {
        qWarning() << "enable foreign keys" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db);
    query.prepare(RATING_LISTS_TABLE_SCHEMA);

    if (!query.exec()) {
        qWarning() << "create schema 1" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db);
    query.prepare(RATING_LIST_PLAYERS_TABLE_SCHEMA);

    if (!query.exec()) {
        qWarning() << "create schema 2" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    return db;
}

std::vector<std::unique_ptr<RatingList>> RatingList::lists()
{
    std::vector<std::unique_ptr<RatingList>> result;

    {
        auto db = RatingList::getDB(RATING_LISTS_DB_CONNECTION_NAME);
        if (!db) {
            return {};
        }

        QSqlQuery query(*db);
        query.prepare(GET_RATING_LISTS_QUERY);

        if (!query.exec()) {
            qWarning() << "error getting rating lists" << query.lastError().text();
            return {};
        }

        int idNo = query.record().indexOf("id");
        int nameNo = query.record().indexOf("name");
        int urlNo = query.record().indexOf("url");
        int etagNo = query.record().indexOf("etag");
        int lastModifiedNo = query.record().indexOf("lastmodified");

        while (query.next()) {
            auto list = std::make_unique<RatingList>();
            list->m_id = query.value(idNo).toInt();
            list->m_name = query.value(nameNo).toString();
            list->m_url = query.value(urlNo).toString();
            list->m_etag = query.value(etagNo).toString();
            list->m_lastModified = query.value(lastModifiedNo).toString();

            result.push_back(std::move(list));
        }

        db->close();
    }

    QSqlDatabase::removeDatabase(RATING_LISTS_DB_CONNECTION_NAME);

    return result;
}

int RatingList::id() const
{
    return m_id;
}

QString RatingList::name() const
{
    return m_name;
}

QCoro::Task<std::expected<void, QString>> RatingList::import(const QString &name, const QUrl &url)
{
    m_name = name;
    m_url = url.toString();

    QNetworkAccessManager manager;

    QNetworkRequest request{url};
    const QString userAgent = "Chessament/"_L1 + QCoreApplication::applicationVersion() + " (https://apps.kde.org/chessament/)"_L1;
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    Q_EMIT statusChanged(i18nc("@info:progress", "Downloading file"));

    auto *reply = manager.get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total) {
        if (total == 0) {
            return;
        }
        const auto progress = 100. * (static_cast<double>(received) / static_cast<double>(total));
        qDebug() << progress;
        Q_EMIT statusChanged(i18nc("@info:progress", "Downloading file (%1 %)", progress));
    });

    co_await qCoro(reply).waitForFinished();

    if (reply->error() != QNetworkReply::NoError) {
        co_return std::unexpected(i18nc("@info", "Couldn't download rating list: %1", reply->errorString()));
    }

    auto result = reply->readAll();

    m_etag = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::ETag));
    m_lastModified = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::LastModified));

    QBuffer buffer(&result);
    auto zip = KZip(&buffer);
    if (!zip.open(QIODevice::ReadOnly)) {
        qWarning() << zip.errorString();
        co_return std::unexpected(i18nc("@info", "Couldn't extract file."));
    }

    const auto directory = zip.directory();
    if (directory->entries().size() != 1) {
        co_return std::unexpected(i18nc("@info", "File format not supported."));
    }

    const auto archiveFile = directory->file(directory->entries().constFirst());
    const auto device = archiveFile->createDevice();

    QTextStream stream(device);

    const auto ok = co_await QtConcurrent::run([this, &stream]() {
        return readPlayers(&stream);
    });

    QSqlDatabase::removeDatabase(RATING_LISTS_DB_CONNECTION_NAME);
    reply->deleteLater();
    device->deleteLater();

    if (!ok) {
        co_return std::unexpected(ok.error());
    }

    Q_EMIT statusChanged(i18ncp("@info:progress", "Imported one player.", "Imported %1 players.", *ok));

    co_return {};
}

std::expected<int, QString> RatingList::readPlayers(QTextStream *stream)
{
    QString line;
    int count = 0;

    QVariantList lists;
    QVariantList names;
    QVariantList ids;
    QVariantList federations;
    QVariantList genders;
    QVariantList titles;
    QVariantList birthdates;
    QVariantList stdRatings;
    QVariantList rpdRatings;
    QVariantList btzRatings;
    QVariantList extras;

    stream->readLine(); // Skip header

    auto db = getDB();
    if (!db) {
        return std::unexpected(db.error());
    }

    if (const auto ok = db->transaction(); !ok) {
        return std::unexpected(db->lastError().text());
    }

    QSqlQuery query(*db);
    query.prepare(ADD_RATING_LIST_QUERY);
    query.bindValue(":name"_L1, m_name);
    query.bindValue(":url"_L1, m_url);
    query.bindValue(":etag"_L1, m_etag);
    query.bindValue(":lastmodified"_L1, m_lastModified);

    if (!query.exec()) {
        qWarning() << "create list" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    m_id = query.lastInsertId().toInt();

    auto addPlayers = [&db, &lists, &names, &ids, &federations, &genders, &titles, &birthdates, &stdRatings, &rpdRatings, &btzRatings, &extras]()
        -> std::expected<void, QString> {
        QSqlQuery query(*db);
        query.prepare(ADD_RATING_LIST_PLAYER_QUERY);
        query.addBindValue(lists);
        query.addBindValue(names);
        query.addBindValue(ids);
        query.addBindValue(federations);
        query.addBindValue(genders);
        query.addBindValue(titles);
        query.addBindValue(birthdates);
        query.addBindValue(stdRatings);
        query.addBindValue(rpdRatings);
        query.addBindValue(btzRatings);
        query.addBindValue(extras);

        if (!query.execBatch()) {
            qWarning() << "create players" << query.lastError().text();
            return std::unexpected(query.lastError().text());
        }

        lists.clear();
        names.clear();
        ids.clear();
        federations.clear();
        genders.clear();
        titles.clear();
        birthdates.clear();
        stdRatings.clear();
        rpdRatings.clear();
        btzRatings.clear();
        extras.clear();

        return {};
    };

    while (stream->readLineInto(&line)) {
        if (line.size() < 162) {
            continue;
        }

        bool ok;

        const int playerId = line.sliced(0, 15).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid player id of player" << line;
            continue;
        }

        const QString name = line.sliced(15, 61).trimmed();
        const QString federation = line.sliced(76, 3);
        const QString gender = line.sliced(80, 3).trimmed();
        const QString title = line.sliced(84, 4).trimmed();
        const auto standardRating = line.sliced(113, 4).toUInt(&ok);
        if (!ok) {
            qWarning() << "invalid standard rating of player" << line;
            continue;
        }

        const auto rapidRating = line.sliced(126, 4).toUInt(&ok);
        if (!ok) {
            qWarning() << "invalid rapid rating of player" << line;
            continue;
        }

        const auto blitzRating = line.sliced(139, 4).toUInt(&ok);
        if (!ok) {
            qWarning() << "invalid blitz rating of player" << line;
            continue;
        }

        const auto otherTitles = line.sliced(94, 15).trimmed().split(u',', Qt::SkipEmptyParts);

        const auto sk = line.sliced(123, 2).toInt(&ok);
        if (!ok) {
            continue;
        }

        const auto rk = line.sliced(136, 2).toInt(&ok);
        if (!ok) {
            continue;
        }

        const auto bk = line.sliced(149, 2).toInt(&ok);
        if (!ok) {
            continue;
        }

        QJsonObject extraJson;
        if (sk != 0) {
            extraJson["sk"_L1] = sk;
        }
        if (rk != 0) {
            extraJson["rk"_L1] = rk;
        }
        if (bk != 0) {
            extraJson["bk"_L1] = bk;
        }
        if (!otherTitles.empty()) {
            extraJson["other_titles"_L1] = QJsonValue::fromVariant(otherTitles);
        }
        const auto extra = QJsonDocument{extraJson}.toJson(QJsonDocument::Compact);

        const auto birthdate = line.sliced(152, 4).toInt(&ok);
        if (!ok) {
            continue;
        }

        lists << m_id;
        names << name;
        ids << playerId;
        federations << federation;
        genders << gender;
        titles << title;
        birthdates << birthdate;
        stdRatings << standardRating;
        rpdRatings << rapidRating;
        btzRatings << blitzRating;
        extras << extra;

        ++count;

        if (count % 100 == 0) {
            if (const auto ok = addPlayers(); !ok) {
                return std::unexpected(ok.error());
            }

            Q_EMIT statusChanged(i18ncp("@info:progress", "Saved 1 player.", "Saved %1 players.", count));
        }
    }

    if (const auto ok = addPlayers(); !ok) {
        return std::unexpected(ok.error());
    }

    if (!db->commit()) {
        return std::unexpected(db->lastError().text());
    }

    db->close();

    return count;
}

void RatingList::remove(int id)
{
    {
        auto db = RatingList::getDB(RATING_LISTS_DB_CONNECTION_NAME);
        if (!db) {
            return;
        }

        QSqlQuery query(*db);
        query.prepare(DELETE_RATING_LIST_QUERY);
        query.bindValue(":id"_L1, id);

        if (!query.exec()) {
            qWarning() << "error deleting rating list" << query.lastError().text();
            return;
        }

        db->close();
    }

    QSqlDatabase::removeDatabase(RATING_LISTS_DB_CONNECTION_NAME);
}
