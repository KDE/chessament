// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QTextStream>

#include <expected>

#include "pairing.h"
#include "player.h"

class Tournament;

class TrfReader
{
public:
    struct pairing {
        int round;
        int white;
        int black;
    };

private:
    explicit TrfReader(Tournament *tournament);

    std::expected<void, QString> read(QTextStream *trf);

    std::expected<void, QString> readField(QStringView line);
    std::expected<void, QString> readDates(QStringView line);
    std::expected<void, QString> readPlayer(QStringView line);
    std::expected<void, QString> readPairing(int startingRank, int round, QStringView text);

    Tournament *m_tournament;

    QHash<int, Player *> m_players;
    QHash<pairing, Pairing::Result> m_pairings;

    friend class Tournament;
};

bool operator==(const TrfReader::pairing &a, const TrfReader::pairing &b) noexcept;

namespace std
{
template<>
struct hash<TrfReader::pairing> {
    size_t operator()(const TrfReader::pairing &key, size_t seed = 0) const
    {
        return qHashMulti(seed, key.round, key.white, key.black);
    }
};
}
