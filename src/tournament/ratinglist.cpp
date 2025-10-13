// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglist.h"

#include <QCoreApplication>
#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QDir>
#include <QNetworkAccessManager>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QtConcurrent>

#include <KLocalizedString>
#include <KZip>

using namespace Qt::StringLiterals;

static constexpr auto RATING_LISTS_DB_CONNECTION_NAME = "rating-lists"_L1;

std::expected<QSqlDatabase, QString> RatingList::getDB()
{
    if (!QSqlDatabase::contains(RATING_LISTS_DB_CONNECTION_NAME)) {
        qDebug() << "opening ratings db";
        const QString dbFolder = QStandardPaths::writableLocation(QStandardPaths::StateLocation);
        QDir().mkpath(dbFolder);

        auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, RATING_LISTS_DB_CONNECTION_NAME);
        db.setDatabaseName(dbFolder + "/rating-lists.db"_L1);

        if (!db.open()) {
            return std::unexpected(db.lastError().text());
        }

        QSqlQuery query(db);
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
    }

    return QSqlDatabase::database(RATING_LISTS_DB_CONNECTION_NAME);
}

QCoro::Task<std::expected<std::unique_ptr<RatingList>, QString>> RatingList::importList(const QString &name, const QUrl &url)
{
    {
        auto db = getDB();
        if (!db) {
            co_return std::unexpected(db.error());
        }

        QSqlQuery query(*db);
        query.prepare(ADD_RATING_LIST_QUERY);
        query.bindValue(u":name"_s, name);
        query.bindValue(u":url"_s, "x"_L1);
        query.bindValue(u":etag"_s, "xx"_L1);
        query.bindValue(u":lastmodified"_s, "x"_L1);

        if (!query.exec()) {
            qWarning() << "create list" << query.lastError().text();
            db->close();
            co_return std::unexpected(query.lastError().text());
        }

        m_id = query.lastInsertId().toInt();
    }

    QSqlDatabase::removeDatabase(RATING_LISTS_DB_CONNECTION_NAME);

    if (const auto ok = co_await import(url); !ok) {
        co_return std::unexpected(ok.error());
    }

    co_return {};
}

QCoro::Task<std::expected<void, QString>> RatingList::import(const QUrl &url)
{
    QNetworkAccessManager manager;

    QNetworkRequest request{url};
    const QString userAgent = "Chessament/"_L1 + QCoreApplication::applicationVersion() + " (https://apps.kde.org/chessament/)"_L1;
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    Q_EMIT statusChanged(i18nc("@info:status", "Downloading file"));

    auto *reply = co_await manager.get(request);

    qDebug() << reply->headers();

    QTemporaryFile file;
    file.open();
    file.write(reply->readAll());
    reply->deleteLater();

    auto zip = KZip(file.fileName());
    zip.open(QIODevice::ReadOnly);

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

    if (!ok) {
        co_return std::unexpected(ok.error());
    }

    Q_EMIT statusChanged(i18ncp("@info:status", "Imported one player.", "Imported %1 players.", *ok));

    device->deleteLater();

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
    QVariantList stdRatings;
    QVariantList rpdRatings;
    QVariantList btzRatings;

    // Skip header
    stream->readLine();

    auto db = getDB();
    if (!db) {
        return std::unexpected(db.error());
    }

    db->transaction();

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
        const int standardRating = line.sliced(113, 4).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid standard rating of player" << line;
            continue;
        }

        const int rapidRating = line.sliced(126, 4).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid rapid rating of player" << line;
            continue;
        }

        const int blitzRating = line.sliced(139, 4).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid blitz rating of player" << line;
            continue;
        }

        lists << m_id;
        names << name;
        ids << playerId;
        federations << federation;
        stdRatings << standardRating;
        rpdRatings << rapidRating;
        btzRatings << blitzRating;

        ++count;

        if (count % 100 == 0) {
            QSqlQuery query(*db);
            query.prepare(ADD_RATING_LIST_PLAYER_QUERY);
            query.addBindValue(lists);
            query.addBindValue(names);
            query.addBindValue(ids);
            query.addBindValue(federations);
            query.addBindValue(stdRatings);
            query.addBindValue(rpdRatings);
            query.addBindValue(btzRatings);

            if (!query.execBatch()) {
                qWarning() << "create players" << query.lastError().text();
                return std::unexpected(query.lastError().text());
            }

            Q_EMIT statusChanged(i18ncp("@info:status", "Saved 1 player", "Saved %1 players", count));

            lists.clear();
            names.clear();
            ids.clear();
            federations.clear();
            stdRatings.clear();
            rpdRatings.clear();
            btzRatings.clear();
        }
    }

    qDebug() << "read" << count;
    db->commit();

    return count;
}
