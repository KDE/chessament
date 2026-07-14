// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <expected>

#include <QCoroTask>
#include <QJsonObject>
#include <QMimeType>
#include <QUrl>
#include <qqmlregistration.h>

#include "ratinglistplayer.h"

class QSqlDatabase;
class RatingListReader;

using namespace Qt::StringLiterals;

constexpr auto RATING_LISTS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS ratinglists("
    "id INTEGER PRIMARY KEY,"
    "name TEXT NOT NULL,"
    "url TEXT,"
    "etag TEXT,"
    "lastModified TEXT,"
    "extra BLOB"
    ");"_L1;

constexpr auto ADD_RATING_LIST_QUERY =
    "INSERT INTO ratinglists(name, url, etag, lastModified, extra) "
    "VALUES (:name, :url, :etag, :lastModified, :extra);"_L1;

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

constexpr auto ADD_RATING_LIST_PLAYER_QUERY =
    "INSERT INTO players(list, name, playerId, federation, gender, title, birthday, standard, rapid, blitz, nationalId, nationalRating, extra) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, jsonb(?));"_L1;

static const auto SEARCH_PLAYERS_QUERY =
    u"SELECT playerId, name, federation, gender, title, birthday, standard, rapid, blitz, nationalId, nationalRating, json(extra) as extra FROM players WHERE name LIKE :search LIMIT 20;"_s;

static constexpr auto RATING_LISTS_DB_CONNECTION_NAME = "rating-lists"_L1;
static constexpr auto RATING_LISTS_DB_CONNECTION_NAME_WRITER = "rating-lists-writer"_L1;

class RatingList : public QObject
{
    Q_OBJECT

public:
    explicit RatingList() = default;

    explicit RatingList(QString name);

    static std::vector<std::unique_ptr<RatingList>> lists();

    [[nodiscard]] int id() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QByteArray extraString() const;

    QCoro::Task<std::expected<void, QString>> import(const QUrl &url);

    std::expected<uint, QString> readPlayers(QTextStream *stream, std::unique_ptr<RatingListReader> reader);

    static QString databasePath();

    static void remove(int id);

    static std::expected<QList<RatingListPlayer>, QString> searchPlayers(const QString &text);

public Q_SLOTS:
    void setExtra(const QByteArray &extra);

Q_SIGNALS:
    void statusChanged(const QString &status);

private:
    static QString databaseFolder();

    static std::expected<QSqlDatabase, QString> getDb(const QString &connectionName = RATING_LISTS_DB_CONNECTION_NAME);

    std::expected<uint, QString> processFile(QByteArray content, const QMimeType &mime);
    std::expected<void, QString> savePlayers(const QList<RatingListPlayer> &players);

    int m_id{};
    QString m_name;
    QString m_url;
    QString m_etag;
    QString m_lastModified;
    QJsonObject m_extra;

    uint m_playerCount{0};

    friend class RatingListReader;
};
