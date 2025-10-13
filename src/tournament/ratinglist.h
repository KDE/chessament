// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <expected>

#include <QCoroTask>
#include <QUrl>

using namespace Qt::StringLiterals;

constexpr QLatin1StringView RATING_LISTS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS ratinglists("
    "name TEXT,"
    "url TEXT NOT NULL,"
    "etag TEXT,"
    "last_modified TEXT"
    ");"_L1;

constexpr QLatin1StringView RATING_LIST_PLAYERS_TABLE_SCHEMA =
    "CREATE TABLE IF NOT EXISTS players("
    "name TEXT,"
    "playerid TEXT,"
    "standard INTEGER,"
    "rapid INTEGER,"
    "blitz INTEGER"
    ");"_L1;

class RatingList
{
public:
    static QCoro::Task<std::expected<std::unique_ptr<RatingList>, QString>> importList(const QUrl &url);

private:
    struct Player {
        int playerId;
        QString name;
        QString federation;
        int standardRating;
        int rapidRating;
        int blitzRating;
    };

    QCoro::Task<std::expected<void, QString>> import(const QUrl &url);
    std::expected<void, QString> readPlayers(QTextStream *stream);
};
