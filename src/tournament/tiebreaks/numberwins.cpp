// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "numberwins.h"
#include "state.h"

double NumberOfWins::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    int result = 0;

    for (const auto &pairing : state.pairings(player)) {
        if (pairing->pointsOfPlayer(player) == 1.) {
            ++result;
        }
    }

    return result;
}
