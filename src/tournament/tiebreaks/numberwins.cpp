// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "numberwins.h"
#include "state.h"

double NumberOfWins::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    const auto pairings = state.pairings(player);

    const auto result = std::ranges::count_if(pairings, [&player](Pairing *pairing) {
        return pairing->pointsOfPlayer(player) == 1.;
    });

    return static_cast<double>(result);
}
