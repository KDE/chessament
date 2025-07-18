// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#include "tiebreaks.h"
#include "tiebreak.h"
#include "tournamentstate.h"

QVariant Tiebreak::option(const QString &key, const QVariant &defaultValue) const
{
    return m_options.value(key, defaultValue);
}

QMap<QString, QVariantMap> Tiebreak::availableOptions() const
{
    return {};
}

double Points::calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    return state.getPoints(player);
}

double Buchholz::calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player)
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
