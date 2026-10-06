// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "progressive.h"
#include "state.h"

double Progressive::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    double points{0.};
    double acc{0.};

    const auto pairings = state.pairings(player);
    for (const auto &pairing : pairings) {
        points += pairing->pointsOfPlayer(player);
        acc += points;
    }

    return acc;
}
