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
    "list TEXT NOT NULL,"
    "name TEXT,"
    "playerid TEXT,"
    "federation TEXT,"
    "gender TEXT,"
    "title TEXT,"
    "birthday INTEGER,"
    "standard INTEGER,"
    "rapid INTEGER,"
    "blitz INTEGER,"
    "extra BLOB,"
    "FOREIGN KEY (list) REFERENCES ratinglists(id) ON DELETE CASCADE"
    ");"_L1;

constexpr auto ADD_RATING_LIST_PLAYER_QUERY =
    "INSERT INTO players(list, name, playerid, federation, gender, title, birthday, standard, rapid, blitz, extra) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, jsonb(?));"_L1;

static constexpr auto RATING_LISTS_DB_CONNECTION_NAME = "rating-lists"_L1;

class RatingList : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    static std::vector<std::unique_ptr<RatingList>> lists();

    [[nodiscard]] int id() const;
    [[nodiscard]] QString name() const;

    QCoro::Task<std::expected<void, QString>> import(const QString &name, const QUrl &url);

    static void remove(int id);

Q_SIGNALS:
    void statusChanged(const QString &status);

private:
    static std::expected<QSqlDatabase, QString> getDB(const QString &connectionName = RATING_LISTS_DB_CONNECTION_NAME);

    std::expected<int, QString> readPlayers(QTextStream *stream);

    int m_id;
    QString m_name;
    QString m_url;
    QString m_etag;
    QString m_lastModified;
};
