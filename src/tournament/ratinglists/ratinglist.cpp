// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
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
#include "fidereader.h"
#include "reader.h"

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
    db.setDatabaseName(dbFolder + "/ratinglists.db"_L1);

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
    const QString userAgent = "Chessament/"_L1 + QCoreApplication::applicationVersion() + " (+https://apps.kde.org/chessament/)"_L1;
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

    m_etag = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::ETag));
    m_lastModified = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::LastModified));

    const auto result = reply->readAll();
    const auto count = co_await QtConcurrent::run([this, &result]() -> std::expected<uint, QString> {
        return processFile(result);
    });

    QSqlDatabase::removeDatabase(RATING_LISTS_DB_CONNECTION_NAME);
    reply->deleteLater();

    if (!count) {
        co_return std::unexpected(count.error());
    }

    Q_EMIT statusChanged(i18ncp("@info:progress", "Imported one player.", "Imported %1 players.", *count));

    co_return {};
}

std::expected<uint, QString> RatingList::processFile(QByteArray content)
{
    QBuffer buffer(&content);
    auto zip = KZip(&buffer);
    if (!zip.open(QIODevice::ReadOnly)) {
        qWarning() << zip.errorString();
        return std::unexpected(i18nc("@info", "Couldn't extract file."));
    }

    const auto directory = zip.directory();
    if (directory->entries().size() != 1) {
        return std::unexpected(i18nc("@info", "File format not supported."));
    }

    const auto archiveFile = directory->file(directory->entries().constFirst());
    const auto device = archiveFile->createDevice();
    device->deleteLater();

    QTextStream stream(device);

    const auto count = readPlayers(&stream);
    if (!count) {
        return std::unexpected(count.error());
    }

    return *count;
}

std::expected<uint, QString> RatingList::readPlayers(QTextStream *stream)
{
    QString line;

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

    std::unique_ptr<RatingListReader> reader;
    reader = std::make_unique<FideRatingListReader>(this);

    const auto count = reader->readPlayers(stream);
    if (!count) {
        return std::unexpected(count.error());
    }

    if (!db->commit()) {
        return std::unexpected(db->lastError().text());
    }

    db->close();

    return *count;
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

std::expected<void, QString> RatingList::savePlayers(const QList<RatingList::Player> &players)
{
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
    QVariantList nationalIds;
    QVariantList nationalRatings;
    QVariantList extras;

    auto db = RatingList::getDB();
    if (!db) {
        return std::unexpected(db.error());
    }

    for (const auto &player : players) {
        lists << m_id;
        names << player.name;
        ids << player.id;
        federations << player.federation;
        genders << player.gender;
        titles << player.title;
        birthdates << player.birthDate;
        stdRatings << player.standardRating;
        rpdRatings << player.rapidRating;
        btzRatings << player.blitzRating;
        nationalIds << player.nationalId;
        nationalRatings << player.nationalRating;

        const auto extra = QJsonDocument{player.extra}.toJson(QJsonDocument::Compact);

        extras << extra;
    }

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
    query.addBindValue(nationalIds);
    query.addBindValue(nationalRatings);
    query.addBindValue(extras);

    if (!query.execBatch()) {
        qWarning() << "create players" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    m_playerCount += players.size();

    Q_EMIT statusChanged(i18ncp("@info:progress", "Saved 1 player.", "Saved %1 players.", m_playerCount));

    return {};
}

#include "moc_ratinglist.cpp"
