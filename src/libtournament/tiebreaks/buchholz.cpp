// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#include "buchholz.h"
#include "pairing.h"
#include "state.h"

QList<QVariantMap> Buchholz::options()
{
    return {}; // TODO: implement modifiers
    /*return {
        {
            {"id"_L1, "ignore_lowest"_L1},
            {"name"_L1, "Ignore the lowest values"_L1},
            {"type"_L1, "number"_L1},
            {"value"_L1, option("ignore_lowest"_L1, 1)},
        },
        {
            {"id"_L1, "forfeit_regular"_L1},
            {"name"_L1, "Forfeited games as played games"_L1},
            {"type"_L1, "checkbox"_L1},
            {"value"_L1, option("forfeit_regular"_L1, false)},
        },
    };*/
}

double Buchholz::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    double result = 0.;
    for (const auto &pairing : state.getPairings(player)) {
        double p;
        Player *opponent;

        if (pairing->whitePlayer() == player) {
            opponent = pairing->blackPlayer();
        } else {
            opponent = pairing->whitePlayer();
        }

        // Handle unplayed rounds of player
        if (Pairing::isUnplayed(pairing->whiteResult())) {
            // 16.4: dummy opponent with the same points as the player
            p = state.getPoints(player);
        } else {
            p = state.getPointsForTiebreaks(opponent);
        }

        result += p;
    }
    return result;
};
