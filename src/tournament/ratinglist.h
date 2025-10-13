// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <expected>

#include <QCoroTask>
#include <QUrl>
#include <qqmlregistration.h>

class QSqlDatabase;

using namespace Qt::StringLiterals;

constexpr auto RATING_LISTS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS ratinglists("
    "name TEXT,"
    "url TEXT NOT NULL,"
    "etag TEXT,"
    "lastmodified TEXT"
    ");"_L1;

constexpr auto ADD_RATING_LIST_QUERY =
    "INSERT INTO ratinglists(name, url, etag, lastmodified) "
    "VALUES (:name, :url, :etag, :lastmodified);"_L1;

constexpr auto RATING_LIST_PLAYERS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS players("
    "list INTEGER NOT NULL,"
    "name TEXT,"
    "playerid TEXT,"
    "federation TEXT,"
    "standard INTEGER,"
    "rapid INTEGER,"
    "blitz INTEGER,"
    "FOREIGN KEY (list) REFERENCES ratinglists(id)"
    ");"_L1;

constexpr auto ADD_RATING_LIST_PLAYER_QUERY =
    "INSERT INTO players(list, name, playerid, federation, standard, rapid, blitz) "
    "VALUES (?, ?, ?, ?, ?, ?, ?);"_L1;

class RatingList : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    QCoro::Task<std::expected<std::unique_ptr<RatingList>, QString>> importList(const QString &name, const QUrl &url);

Q_SIGNALS:
    void statusChanged(const QString &status);

private:
    struct Player {
        int playerId;
        QString name;
        QString federation;
        int standardRating;
        int rapidRating;
        int blitzRating;
    };

    static std::expected<QSqlDatabase, QString> getDB();

    QCoro::Task<std::expected<void, QString>> import(const QUrl &url);
    std::expected<int, QString> readPlayers(QTextStream *stream);

    int m_id;
};
