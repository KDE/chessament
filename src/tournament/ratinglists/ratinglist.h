// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <expected>

#include <QCoroTask>
#include <QJsonObject>
#include <QMimeType>
#include <QUrl>
#include <qqmlregistration.h>

class QSqlDatabase;
class RatingListReader;

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

static const auto SEARCH_PLAYERS_QUERY =
    u"SELECT playerid, name, federation, gender, title, birthday, standard, rapid, blitz, nationalid, nationalrating, json(extra) as extra FROM players WHERE name LIKE :search LIMIT 20;"_s;

static constexpr auto RATING_LISTS_DB_CONNECTION_NAME = "rating-lists"_L1;

struct RatingListPlayer {
    Q_GADGET

    Q_PROPERTY(QString id MEMBER id CONSTANT)
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString federation MEMBER federation CONSTANT)
    Q_PROPERTY(QString gender MEMBER gender CONSTANT)
    Q_PROPERTY(QString title MEMBER title CONSTANT)
    Q_PROPERTY(QString birthDate MEMBER birthDate CONSTANT)
    Q_PROPERTY(uint standardRating MEMBER standardRating CONSTANT)
    Q_PROPERTY(uint rapidRating MEMBER rapidRating CONSTANT)
    Q_PROPERTY(uint blitzRating MEMBER blitzRating CONSTANT)
    Q_PROPERTY(QString nationalId MEMBER nationalId CONSTANT)
    Q_PROPERTY(uint nationalRating MEMBER nationalRating CONSTANT)
    Q_PROPERTY(QJsonObject extra MEMBER extra CONSTANT)

    Q_PROPERTY(QString origin READ origin CONSTANT)

public:
    [[nodiscard]] QString origin() const
    {
        return extra.value("origin"_L1).toString();
    }

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

class RatingList : public QObject
{
    Q_OBJECT

public:
    static std::vector<std::unique_ptr<RatingList>> lists();

    [[nodiscard]] int id() const;
    [[nodiscard]] QString name() const;

    QCoro::Task<std::expected<void, QString>> import(const QString &name, const QUrl &url);

    static void remove(int id);

    static std::expected<QList<RatingListPlayer>, QString> searchPlayers(const QString &text);

    /*!
     * \internal
     */
    std::expected<void, QString> savePlayers(const QList<RatingListPlayer> &players);

Q_SIGNALS:
    void statusChanged(const QString &status);

private:
    static std::expected<QSqlDatabase, QString> getDB(const QString &connectionName = RATING_LISTS_DB_CONNECTION_NAME);

    std::expected<uint, QString> processFile(QByteArray content, const QMimeType &mime);
    std::expected<uint, QString> readPlayers(QTextStream *stream, std::unique_ptr<RatingListReader> reader);

    int m_id;
    QString m_name;
    QString m_url;
    QString m_etag;
    QString m_lastModified;

    uint m_playerCount{0};

    friend class RatingListReader;
};
