// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "playedblack.h"
#include "state.h"

double NumberOfGamesPlayedWithBlack::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    const auto pairings = state.pairings(player);

    const auto result = std::ranges::count_if(pairings, [&player](Pairing *pairing) {
        return pairing->colorOfPlayer(player) == Pairing::Color::Black && !Pairing::isUnplayed(pairing->resultOfPlayer(player));
    });

    return static_cast<double>(result);
}
