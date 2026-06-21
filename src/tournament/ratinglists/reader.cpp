// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "reader.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

RatingListReader::RatingListReader(RatingList *list)
    : m_list(list)
{
}

RatingList *RatingListReader::list()
{
    return m_list;
}

uint RatingListReader::count() const
{
    return m_count;
}

std::expected<void, QString> RatingListReader::addPlayer(const RatingListPlayer &player)
{
    m_players << player;
    ++m_count;

    if (m_count % 100 == 0) {
        return savePlayers();
    }

    return {};
}

std::expected<void, QString> RatingListReader::savePlayers()
{
    if (const auto ok = m_list->savePlayers(m_players); !ok) {
        return ok;
    }

    m_players.clear();

    return {};
}
