// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglistsmanager.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QDir>
#include <QMimeDatabase>
#include <QNetworkAccessManager>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QtConcurrentRun>

#include <KFormat>
#include <KLocalizedString>
#include <KZip>

#include "fidereader.h"
#include "htmlreader.h"
#include "reader.h"
#include "utils.h"

RatingListsManager &RatingListsManager::instance()
{
    static RatingListsManager _instance;
    return _instance;
}

RatingListsManager::RatingListsManager() = default;

QString RatingListsManager::databaseFolder()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString RatingListsManager::databasePath()
{
    return databaseFolder() % "/ratinglists.sqlite3"_L1;
}

std::expected<QSqlDatabase, QString> RatingListsManager::database(const QString &connectionName)
{
    const auto name = connectionName % u'-' % QString::number(reinterpret_cast<intptr_t>(QThread::currentThread()), 16);

    if (QSqlDatabase::contains(name)) {
        return QSqlDatabase::database(name);
    }

    return openDatabase(name);
}

std::expected<QSqlDatabase, QString> RatingListsManager::openDatabase(const QString &connectionName)
{
    qDebug() << "Opening ratings database" << connectionName;
    const QString dbFolder = RatingListsManager::databaseFolder();
    QDir().mkpath(dbFolder);

    const auto dbPath = databasePath();
    qDebug() << "Ratings database location is" << dbPath;

    auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, connectionName);
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        return std::unexpected(db.lastError().text());
    }

    QSqlQuery query(db);
    query.prepare(ENABLE_WAL_QUERY);
    if (!query.exec()) {
        qWarning() << "enable wal" << query.lastError().text();
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

    query = QSqlQuery(db);
    query.prepare(RATING_LIST_PLAYERS_ID_INDEX);

    if (!query.exec()) {
        qWarning() << "Error creating ratings database idx_player_id" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db);
    query.prepare(RATING_LIST_PLAYERS_LIST_INDEX);

    if (!query.exec()) {
        qWarning() << "Error creating ratings database idx_player_list" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db);
    query.prepare(RATING_LIST_PLAYERS_NATIONAL_ID_INDEX);

    if (!query.exec()) {
        qWarning() << "Error creating ratings database idx_national_id" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    return db;
}

std::vector<std::unique_ptr<RatingList>> RatingListsManager::lists()
{
    auto db = database();
    if (!db) {
        qWarning() << db.error();
        return {};
    }

    std::vector<std::unique_ptr<RatingList>> result;

    QSqlQuery query(*db);
    query.prepare(GET_RATING_LISTS_QUERY);

    if (!query.exec()) {
        qWarning() << "error getting rating lists" << query.lastError().text();
        return {};
    }

    const int idNo = query.record().indexOf("id");
    const int nameNo = query.record().indexOf("name");
    const int urlNo = query.record().indexOf("url");
    const int lastModifiedNo = query.record().indexOf("lastModified");
    const int extraNo = query.record().indexOf("extra");

    while (query.next()) {
        auto list = std::make_unique<RatingList>();
        list->setId(query.value(idNo).toInt());
        list->setName(query.value(nameNo).toString());
        list->setUrl(query.value(urlNo).toString());
        list->setLastModified(query.value(lastModifiedNo).toString());
        list->setExtra(query.value(extraNo).toByteArray());

        result.push_back(std::move(list));
    }

    return result;
}

QCoro::Task<std::expected<RatingList *, QString>> RatingListsManager::import(const QString &name, const QUrl &url)
{
    qDebug() << "Importing rating list from" << url;

    if (!url.isValid()) {
        co_return std::unexpected(i18nc("@info", "Could not open file."));
    }

    auto list = new RatingList();
    list->setName(name);
    list->setUrl(url.toString());

    QByteArray result;
    QMimeDatabase mimeDb;
    QMimeType mimeType;

    if (url.scheme() == u"file"_s) {
        if (!QFile::exists(url.toLocalFile())) {
            co_return std::unexpected(i18nc("@info", "Could not open file: the file does not exists."));
        }

        QFile file{url.toLocalFile()};
        if (!file.open(QIODeviceBase::ReadOnly)) {
            co_return std::unexpected(i18nc("@info", "Could not open file."));
        }

        result = file.readAll();
        mimeType = mimeDb.mimeTypeForFile(url.toLocalFile());
        file.close();
    } else if (url.scheme() == u"https"_s) {
        auto manager = Utils::networkAccessManager();

        QNetworkRequest request{url};
        request.setHeader(QNetworkRequest::UserAgentHeader, Utils::userAgent());

        Q_EMIT statusChanged(i18nc("@info:progress", "Downloading file…"));

        auto *reply = manager->get(request);

        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal) {
            if (bytesTotal <= 0) {
                return;
            }
            KFormat format{};
            const auto received = format.formatByteSize(static_cast<double>(bytesReceived));
            const auto total = format.formatByteSize(static_cast<double>(bytesTotal));
            const auto progress = std::round(100. * (static_cast<double>(bytesReceived) / static_cast<double>(bytesTotal)));
            Q_EMIT statusChanged(i18nc("@info:progress %1 & %2 are the file download progress (received / total), %3 is the download percentage",
                                       "Downloading file…\n%1 / %2 (%3%)",
                                       received,
                                       total,
                                       progress));
        });

        co_await qCoro(reply).waitForFinished();

        reply->deleteLater();

        if (reply->error() != QNetworkReply::NetworkError::NoError) {
            co_return std::unexpected(i18nc("@info", "Could not download rating list: %1", reply->errorString()));
        }

        const auto contentType = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::ContentType));
        mimeType = mimeDb.mimeTypeForName(contentType);

        list->extra()["http_etag"_L1] = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::ETag));
        list->extra()["http_last_modified"_L1] = QString::fromLatin1(reply->headers().value(QHttpHeaders::WellKnownHeader::LastModified));

        result = reply->readAll();
    } else {
        co_return std::unexpected(i18nc("@info", "Could not download rating list from %1 (unsupported protocol).", url.toString()));
    }

    const auto count = co_await QtConcurrent::run([list, &result, mimeType]() -> std::expected<uint, QString> {
        return processFile(list, result, mimeType);
    });

    if (!count) {
        co_return std::unexpected(count.error());
    }

    Q_EMIT statusChanged(i18ncp("@info:progress", "Imported one player.", "Imported %1 players.", *count));

    co_return list;
}

std::expected<uint, QString> RatingListsManager::processFile(RatingList *list, QByteArray content, const QMimeType &mime)
{
    std::expected<uint, QString> count;

    QBuffer buffer(&content);
    std::unique_ptr<RatingListReader> reader;

    if (mime.inherits(u"application/zip"_s)) {
        auto zip = KZip(&buffer);
        if (!zip.open(QIODevice::ReadOnly)) {
            qWarning() << zip.errorString();
            return std::unexpected(i18nc("@info", "Could not extract file."));
        }

        const auto directory = zip.directory();
        if (directory->entries().size() != 1) {
            return std::unexpected(i18nc("@info", "File format not supported."));
        }

        const auto archiveFile = directory->file(directory->entries().constFirst());
        const auto device = archiveFile->createDevice();
        QTextStream stream{device};
        stream.setDevice(device);
        device->deleteLater();

        reader = std::make_unique<FideRatingListReader>(list);
        count = readPlayers(list, &stream, std::move(reader));
    } else if (mime.inherits(u"application/vnd.ms-excel"_s)) {
        buffer.open(QBuffer::ReadOnly);
        QTextStream stream{&buffer};

        reader = std::make_unique<HtmlRatingListReader>(list);
        count = readPlayers(list, &stream, std::move(reader));
    }

    if (!count) {
        return std::unexpected(count.error());
    }

    return *count;
}

std::expected<uint, QString> RatingListsManager::readPlayers(RatingList *list, QTextStream *stream, std::unique_ptr<RatingListReader> reader)
{
    auto db = database();
    if (!db) {
        return std::unexpected(db.error());
    }

    if (const auto ok = db->transaction(); !ok) {
        return std::unexpected(db->lastError().text());
    }

    QSqlQuery query(*db);
    query.prepare(ADD_RATING_LIST_QUERY);
    query.bindValue(":name"_L1, list->name());
    query.bindValue(":url"_L1, list->url());
    query.bindValue(":lastModified"_L1, list->lastModified());
    query.bindValue(u":extra"_s, list->extraString());

    if (!query.exec()) {
        qWarning() << "create list" << query.lastError().text();
        return std::unexpected(query.lastError().text());
    }

    list->setId(query.lastInsertId().toInt());

    if (const auto ok = reader->readPlayers(stream); !ok) {
        return std::unexpected(ok.error());
    }

    if (const auto ok = reader->savePlayers(); !ok) {
        return std::unexpected(ok.error());
    }

    if (!db->commit()) {
        return std::unexpected(db->lastError().text());
    }

    return reader->count();
}

void RatingListsManager::remove(int id)
{
    qDebug() << "Starting to remove rating list" << id;

    auto db = database();
    if (!db) {
        qWarning() << db.error();
        return;
    }

    if (const auto ok = db->transaction(); !ok) {
        qWarning() << "Error starting transaction" << db->lastError();
        return;
    }

    QSqlQuery query(*db);
    query.prepare(DELETE_RATING_LIST_PLAYERS_QUERY);
    query.bindValue(":list"_L1, id);

    if (!query.exec()) {
        qWarning() << "Error deleting players from rating list" << query.lastError().text();
        return;
    }

    query = QSqlQuery(*db);
    query.prepare(DELETE_RATING_LIST_QUERY);
    query.bindValue(":id"_L1, id);

    if (!query.exec()) {
        qWarning() << "Error deleting rating list" << query.lastError().text();
        return;
    }

    if (!db->commit()) {
        qWarning() << "Error commiting transaction" << db->lastError();
        return;
    }

    qDebug() << "Finished removing rating list" << id;
}

std::expected<QList<RatingListPlayer>, QString> RatingListsManager::searchPlayers(const QString &text)
{
    auto db = database();
    if (!db) {
        return std::unexpected(db.error());
    }

    QSqlQuery query(*db);
    query.prepare(SEARCH_PLAYERS_QUERY);
    query.bindValue(u":search"_s, u"%%1%"_s.arg(text));

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    return loadPlayers(query);
}

std::optional<RatingListPlayer> RatingListsManager::searchPlayer(const QString &playerId, int listId)
{
    auto db = database();
    if (!db) {
        return std::nullopt;
    }

    QSqlQuery query(*db);
    query.prepare(SEARCH_PLAYER_QUERY);
    query.bindValue(u":playerId"_s, playerId);
    query.bindValue(u":listId"_s, listId);

    if (!query.exec()) {
        return std::nullopt;
    }

    const auto players = loadPlayers(query);

    if (players.isEmpty()) {
        return std::nullopt;
    }

    return players.first();
}

std::expected<void, QString> RatingListsManager::savePlayers(RatingList *list, const QList<RatingListPlayer> &players)
{
    Q_ASSERT(list->id() > 0);

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

    auto db = database();
    if (!db) {
        return std::unexpected(db.error());
    }

    for (const auto &player : players) {
        lists << list->id();
        names << player.name();
        ids << player.id();
        federations << player.federation();
        genders << player.gender();
        titles << player.title();
        birthdates << player.birthDate();
        stdRatings << player.standardRating();
        rpdRatings << player.rapidRating();
        btzRatings << player.blitzRating();
        nationalIds << player.nationalId();
        nationalRatings << player.nationalRating();
        extras << player.extraString();
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

QList<RatingListPlayer> RatingListsManager::loadPlayers(QSqlQuery &query)
{
    QList<RatingListPlayer> players{};

    const int idNo = query.record().indexOf("playerId");
    const int nameNo = query.record().indexOf("name");
    const int federationNo = query.record().indexOf("federation");
    const int genderNo = query.record().indexOf("gender");
    const int titleNo = query.record().indexOf("title");
    const int birthDayNo = query.record().indexOf("birthday");
    const int standardNo = query.record().indexOf("standard");
    const int rapidNo = query.record().indexOf("rapid");
    const int blitzNo = query.record().indexOf("blitz");
    const int nationalIdNo = query.record().indexOf("nationalId");
    const int nationalRatingNo = query.record().indexOf("nationalRating");
    const int extraNo = query.record().indexOf("extra");

    while (query.next()) {
        const auto id = query.value(idNo).toString();
        const auto name = query.value(nameNo).toString();
        const auto federation = query.value(federationNo).toString();
        const auto gender = query.value(genderNo).toString();
        const auto title = query.value(titleNo).toString();
        const auto birthDate = query.value(birthDayNo).toString();
        const auto standardRating = query.value(standardNo).toInt();
        const auto rapidRating = query.value(rapidNo).toInt();
        const auto blitzRating = query.value(blitzNo).toInt();
        const auto nationalId = query.value(nationalIdNo).toString();
        const auto nationalRating = query.value(nationalRatingNo).toInt();

        const auto extra = query.value(extraNo).toByteArray();
        const auto extraJson = QJsonDocument::fromJson(extra);

        const auto player = RatingListPlayer{
            id,
            name,
            federation,
            gender,
            title,
            birthDate,
            standardRating,
            rapidRating,
            blitzRating,
            nationalId,
            nationalRating,
            extraJson.object(),
        };

        players << player;
    }

    return players;
}

#include "moc_ratinglistsmanager.cpp"
