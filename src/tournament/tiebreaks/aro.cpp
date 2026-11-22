// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "aro.h"

#include "state.h"

double AverageRatingOfOpponents::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    const auto pairings = state.pairings(player);

    int count{};
    int rating{};

    for (const auto &pairing : pairings) {
        if (!Pairing::isUnplayed(pairing->resultOfPlayer(player))) {
            const auto r = pairing->opponent(player)->rating();

            rating += r;
            ++count;
        }
    }

    if (count > 0) {
        return std::round(1. * rating / count);
    }

    return 0.;
}
