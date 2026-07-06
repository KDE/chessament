// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QTextStream>

#include <expected>

#include "ratinglist.h"
#include "ratinglistplayer.h"

class RatingListReader
{
public:
    RatingListReader(RatingList *list);

    virtual ~RatingListReader() = default;

    RatingList *list();

    [[nodiscard]] uint count() const;

    virtual std::expected<void, QString> readPlayers(QTextStream *stream) = 0;

    std::expected<void, QString> addPlayer(const RatingListPlayer &player);

private:
    std::expected<void, QString> savePlayers();

    RatingList *m_list;

    uint m_count = 0;
    QList<RatingListPlayer> m_players;

    friend class RatingList;
};
