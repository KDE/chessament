// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "won.h"
#include "state.h"

double NumberOfGamesWon::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    int result = 0;

    const auto pairings = state.pairings(player);
    for (const auto &pairing : pairings) {
        if (pairing->pointsOfPlayer(player) == 1. && !Pairing::isUnplayed(pairing->resultOfPlayer(player))) {
            ++result;
        }
    }

    return result;
}
