// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "aob.h"
#include "pairing.h"
#include "state.h"

double AverageBuchholzOfOpponents::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    QHash<Player *, double> cache;

    auto calculateBuchholz = [&state, &cache](Player *p) -> double {
        if (cache.contains(p)) {
            return cache[p];
        }

        double result = 0.;
        for (const auto pairing : state.pairings(p)) {
            const auto opponent = pairing->opponent(p);

            // Handle unplayed rounds of player
            if (Pairing::isUnplayed(pairing->whiteResult())) {
                // 16.4: dummy opponent with the same points as the player
                result += state.points(p);
            } else {
                result += state.pointsForTiebreaks(opponent);
            }
        }

        cache[p] = result;
        return result;
    };

    double result = 0.;
    int count = 0;

    for (const auto &pairing : state.pairings(player)) {
        const auto opponent = pairing->opponent(player);

        if (!Pairing::isUnplayed(pairing->whiteResult())) {
            result += calculateBuchholz(opponent);
            ++count;
        }
    }

    if (count > 0) {
        return std::round(100 * result / count) / 100.;
    }

    return 0.;
};
