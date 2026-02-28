// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <expected>

#include <QCoroTask>
#include <QJsonObject>
#include <QUrl>
#include <qqmlregistration.h>

class QSqlDatabase;

using namespace Qt::StringLiterals;

constexpr auto RATING_LISTS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS ratinglists("
    "id INTEGER PRIMARY KEY,"
    "name TEXT NOT NULL,"
    "url TEXT,"
    "etag TEXT,"
    "lastmodified TEXT"
    ");"_L1;

constexpr auto ADD_RATING_LIST_QUERY =
    "INSERT INTO ratinglists(name, url, etag, lastmodified) "
    "VALUES (:name, :url, :etag, :lastmodified);"_L1;

constexpr auto GET_RATING_LISTS_QUERY = "SELECT * FROM ratinglists;"_L1;

constexpr auto DELETE_RATING_LIST_QUERY = "DELETE FROM ratinglists WHERE id = :id;"_L1;

constexpr auto RATING_LIST_PLAYERS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS players("
    "list INTEGER NOT NULL,"
    "name TEXT,"
    "playerid TEXT,"
    "federation TEXT,"
    "gender TEXT,"
    "title TEXT,"
    "birthday INTEGER,"
    "standard INTEGER,"
    "rapid INTEGER,"
    "blitz INTEGER,"
    "nationalid TEXT,"
    "nationalrating INTEGER,"
    "extra BLOB,"
    "FOREIGN KEY (list) REFERENCES ratinglists(id) ON DELETE CASCADE"
    ");"_L1;

constexpr auto ADD_RATING_LIST_PLAYER_QUERY =
    "INSERT INTO players(list, name, playerid, federation, gender, title, birthday, standard, rapid, blitz, nationalid, nationalrating, extra) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, jsonb(?));"_L1;

static constexpr auto RATING_LISTS_DB_CONNECTION_NAME = "rating-lists"_L1;

class RatingList : public QObject
{
    Q_OBJECT

public:
    struct Player {
        QString id;
        QString name;
        QString federation;
        QString gender;
        QString title;
        QString birthDate;
        uint standardRating;
        uint rapidRating;
        uint blitzRating;
        QString nationalId;
        uint nationalRating;
        QJsonObject extra;
    };

    static std::vector<std::unique_ptr<RatingList>> lists();

    [[nodiscard]] int id() const;
    [[nodiscard]] QString name() const;

    QCoro::Task<std::expected<void, QString>> import(const QString &name, const QUrl &url);

    static void remove(int id);

    /*!
     * \internal
     */
    std::expected<void, QString> savePlayers(const QList<RatingList::Player> &players);

Q_SIGNALS:
    void statusChanged(const QString &status);

private:
    static std::expected<QSqlDatabase, QString> getDB(const QString &connectionName = RATING_LISTS_DB_CONNECTION_NAME);

    std::expected<uint, QString> processFile(QByteArray content);
    std::expected<uint, QString> readPlayers(QTextStream *stream);

    int m_id;
    QString m_name;
    QString m_url;
    QString m_etag;
    QString m_lastModified;

    uint m_playerCount{0};

    friend class RatingListReader;
};
