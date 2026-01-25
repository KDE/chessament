// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "buchholz.h"
#include "pairing.h"
#include "state.h"

QList<QVariantMap> Buchholz::options()
{
    //  TODO: implement modifiers
    return {
        {
            {"id"_L1, "cut_lowest"_L1},
            {"name"_L1, i18nc("@label:spinbox", "Exclude the lowest scores:")},
            {"type"_L1, "number"_L1},
            {"value"_L1, option("cut_lowest"_L1, 0)},
        },
    };
    /*{
        {"id"_L1, "forfeit_regular"_L1},
        {"name"_L1, "Forfeited games as played games"_L1},
        {"type"_L1, "checkbox"_L1},
        {"value"_L1, option("forfeit_regular"_L1, false)},
    },
};*/
}

std::expected<void, QString> Buchholz::setTrfOptions(const QList<QString> &options)
{
    for (const auto &option : options) {
        if (option.startsWith(u'C', Qt::CaseSensitivity::CaseInsensitive)) {
            bool ok;
            const int cutLowest = option.mid(1).toInt(&ok);
            if (!ok || cutLowest <= 0) {
                return std::unexpected(i18nc("@info", "Unsupported tiebreak option \"%1\"", option));
            }
            setOption("cut_lowest"_L1, cutLowest);
        }
    }

    return {};
}

double Buchholz::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    std::vector<double> contributions;
    std::vector<double> vurContributions;

    for (const auto &pairing : state.getPairings(player)) {
        double p;

        const auto opponent = pairing->opponent(player);

        // Handle unplayed rounds of player
        if (Pairing::isUnplayed(pairing->whiteResult())) {
            // 16.4: dummy opponent with the same points as the player
            p = state.getPoints(player);
        } else {
            p = state.getPointsForTiebreaks(opponent);
        }

        if (Pairing::isVUR(pairing->getResultOfPlayer(player))) {
            vurContributions.push_back(p);
        } else {
            contributions.push_back(p);
        };
    }

    uint cutLowest = option("cut_lowest"_L1, 0).toUInt();
    if (cutLowest > 0) {
        // Sort contributions in descending order so we can pop_back the lowest value.
        std::ranges::sort(contributions, std::ranges::greater());
        std::ranges::sort(vurContributions, std::ranges::greater());

        for (uint i = 0; i < cutLowest; ++i) {
            if (!vurContributions.empty()) {
                vurContributions.pop_back();
            } else if (!contributions.empty()) {
                contributions.pop_back();
            }
        }
    }

    return std::accumulate(contributions.cbegin(), contributions.cend(), 0.) + std::accumulate(vurContributions.cbegin(), vurContributions.cend(), 0.);
};
