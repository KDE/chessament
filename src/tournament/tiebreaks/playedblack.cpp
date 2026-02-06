// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "playedblack.h"
#include "state.h"

double NumberOfGamesPlayedWithBlack::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    int result = 0;

    for (const auto &pairing : state.pairings(player)) {
        if (pairing->colorOfPlayer(player) == Pairing::Color::Black && !Pairing::isUnplayed(pairing->resultOfPlayer(player))) {
            ++result;
        }
    }

    return result;
}
