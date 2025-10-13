// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <expected>

#include <QCoroTask>
#include <QUrl>

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
