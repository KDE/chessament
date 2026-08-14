// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ratinglist.h"

#include <QCoroTask>
#include <QSqlDatabase>

#include <expected>

#include "ratinglistplayer.h"
#include "reader.h"

using namespace Qt::StringLiterals;

static const QString ENABLE_WAL_QUERY = u"PRAGMA journal_mode = WAL;"_s;

constexpr auto RATING_LISTS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS ratinglists("
    "id INTEGER PRIMARY KEY,"
    "name TEXT NOT NULL,"
    "url TEXT,"
    "lastModified INTEGER,"
    "extra BLOB"
    ");"_L1;

constexpr auto ADD_RATING_LIST_QUERY =
    "INSERT INTO ratinglists(name, url, lastModified, extra) "
    "VALUES (:name, :url, :lastModified, :extra);"_L1;

constexpr auto GET_RATING_LISTS_QUERY = "SELECT * FROM ratinglists;"_L1;

constexpr auto DELETE_RATING_LIST_QUERY = "DELETE FROM ratinglists WHERE id = :id;"_L1;

constexpr auto RATING_LIST_PLAYERS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS players("
    "list INTEGER NOT NULL,"
    "name TEXT,"
    "playerId TEXT,"
    "federation TEXT,"
    "gender TEXT,"
    "title TEXT,"
    "birthday INTEGER,"
    "standard INTEGER,"
    "rapid INTEGER,"
    "blitz INTEGER,"
    "nationalId TEXT,"
    "nationalRating INTEGER,"
    "extra BLOB,"
    "FOREIGN KEY (list) REFERENCES ratinglists(id) ON DELETE CASCADE"
    ");"_L1;

const QString RATING_LIST_PLAYERS_ID_INDEX = u"CREATE INDEX IF NOT EXISTS idx_player_id ON players(playerId);"_s;

const QString RATING_LIST_PLAYERS_NATIONAL_ID_INDEX = u"CREATE INDEX IF NOT EXISTS idx_national_id ON players(nationalId);"_s;

const QString RATING_LIST_PLAYERS_LIST_INDEX = u"CREATE INDEX IF NOT EXISTS idx_player_list ON players(list);"_s;

constexpr auto ADD_RATING_LIST_PLAYER_QUERY =
    "INSERT INTO players(list, name, playerId, federation, gender, title, birthday, standard, rapid, blitz, nationalId, nationalRating, extra) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, jsonb(?));"_L1;

constexpr auto DELETE_RATING_LIST_PLAYERS_QUERY = "DELETE FROM players WHERE list = :list;"_L1;

static const auto SEARCH_PLAYERS_QUERY =
    u"SELECT playerId, name, federation, gender, title, birthday, standard, rapid, blitz, nationalId, nationalRating, json(extra) as extra FROM players WHERE name LIKE :search LIMIT 20;"_s;

static const QString SEARCH_PLAYER_QUERY =
    u"SELECT playerId, name, federation, gender, title, birthday, standard, rapid, blitz, nationalId, nationalRating, json(extra) as extra "
    "FROM players WHERE list = :listId AND playerId = :playerId LIMIT 1;"_s;

static constexpr auto RATING_LISTS_DB_CONNECTION_NAME = "rating-lists"_L1;
static constexpr auto RATING_LISTS_DB_CONNECTION_NAME_WRITER = "rating-lists-writer"_L1;
static constexpr auto RATING_LISTS_DB_CONNECTION_NAME_READER = "rating-lists-reader"_L1;

class RatingListsManager : public QObject
{
    Q_OBJECT

public:
    static RatingListsManager &instance();

    static QString databasePath();

    static std::expected<QSqlDatabase, QString> database(const QString &connectionName = RATING_LISTS_DB_CONNECTION_NAME);

    static std::vector<std::unique_ptr<RatingList>> lists();

    QCoro::Task<std::expected<RatingList *, QString>> import(const QString &name, const QUrl &url);

    static std::expected<uint, QString> readPlayers(RatingList *list, QTextStream *stream, std::unique_ptr<RatingListReader> reader);

    static void remove(int id);

    static std::expected<QList<RatingListPlayer>, QString> searchPlayers(const QString &text);

    static std::optional<RatingListPlayer> searchPlayer(const QString &playerId, int listId);

Q_SIGNALS:
    void statusChanged(const QString &status);

private:
    explicit RatingListsManager();

    static QString databaseFolder();

    static std::expected<QSqlDatabase, QString> openDatabase(const QString &connectionName);

    static std::expected<uint, QString> processFile(RatingList *list, QByteArray content, const QMimeType &mime);

    std::expected<void, QString> savePlayers(RatingList *list, const QList<RatingListPlayer> &players);

    static QList<RatingListPlayer> loadPlayers(QSqlQuery &query);

    uint m_playerCount{0};

    friend class RatingListReader;
};
