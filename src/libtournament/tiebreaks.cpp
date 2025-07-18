// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#include "tiebreaks.h"
#include "tiebreak.h"
#include "tournamentstate.h"

QVariant Tiebreak::option(const QString &key, const QVariant &defaultValue)
{
    return m_options.value(key, defaultValue);
}

bool Tiebreak::isConfigurable()
{
    return false;
}

QList<QVariantMap> Tiebreak::options()
{
    return {};
}

void Tiebreak::setOptions(const QList<QVariantMap> &options)
{
    for (const auto &option : options) {
        m_options[option["id"_L1].toString()] = option["value"_L1];
    }
}

void Tiebreak::setOptions(const QVariantMap &options)
{
    for (const auto option : options.asKeyValueRange()) {
        m_options[option.first] = option.second;
    }
}

QJsonObject Tiebreak::toJson()
{
    return {
        {"id"_L1, id()},
        {"options"_L1, QJsonObject::fromVariantMap(m_options)},
    };
}

double Points::calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    return state.getPoints(player);
}

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
