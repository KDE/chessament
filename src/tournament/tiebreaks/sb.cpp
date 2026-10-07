// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "sb.h"

#include "pairing.h"
#include "state.h"

QList<QVariantMap> SonnebornBergerBase::options()
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

std::expected<void, QString> SonnebornBergerBase::setTrfOptions(const QList<QString> &options)
{
    for (const auto &option : options) {
        if (option.startsWith(u'C', Qt::CaseSensitivity::CaseInsensitive)) {
            bool ok;
            const int cutLowest = option.mid(1).toInt(&ok);
            if (!ok || cutLowest <= 0) {
                return std::unexpected(i18nc("@info", "Unsupported tiebreak option “%1”", option));
            }
            setOption("cut_lowest"_L1, cutLowest);
        }
    }

    return {};
}

double SonnebornBergerBase::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(players)

    QList<round> contributions;

    const auto pairings = state.pairings(player);
    for (const auto &pairing : pairings) {
        double p;

        const auto opponent = pairing->opponent(player);

        // Handle unplayed rounds of player
        if (Pairing::isUnplayed(pairing->whiteResult())) {
            // 16.4: dummy opponent with the same points as the player
            p = state.points(player);

            if (Pairing::isForfeit(pairing->whiteResult())) {
                p = std::min(p, state.pointsForTiebreaks(opponent));
            } else {
                p = std::min(p, .5 * tournament->numberOfRounds());
            }
        } else {
            p = state.pointsForTiebreaks(opponent);
        }

        round r{.value = p, .contribution = 0., .isVur = Pairing::isVUR(pairing->resultOfPlayer(player))};

        p = contribution(pairing->pointsOfPlayer(player), p);
        r.contribution = p;

        contributions.push_back(r);
    }

    processCuts(contributions);

    return std::accumulate(contributions.cbegin(), contributions.cend(), 0., [](double acc, auto c) {
        return acc + c.contribution;
    });
};

void SonnebornBergerBase::processCuts(QList<round> &contributions)
{
    uint cutLowest = option("cut_lowest"_L1, 0).toUInt();

    if (cutLowest > 0) {
        for (uint i = 0; i < cutLowest; ++i) {
            QList<round> vurs;
            std::ranges::copy_if(contributions, std::back_inserter(vurs), [](const round &r) {
                return r.isVur;
            });

            std::ranges::sort(vurs, [](auto a, auto b) {
                return a.contribution < b.contribution;
            });
            std::ranges::sort(contributions, [](auto a, auto b) {
                if (a.value == b.value) {
                    return a.contribution < b.contribution;
                }
                return a.value < b.value;
            });

            if (!vurs.empty()) {
                if (vurs.first().contribution < contributions.first().contribution) {
                    contributions.removeFirst();
                } else {
                    contributions.removeOne(vurs.first());
                }
            } else if (!contributions.empty()) {
                contributions.removeFirst();
            }
        }
    }
}

double SonnebornBerger::contribution(double pointsForResult, double value) const
{
    return pointsForResult * value;
}
