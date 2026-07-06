// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "state.h"

State::State(Tournament *tournament, std::optional<int> maxRound)
    : m_tournament(tournament)
{
    if (!maxRound) {
        m_maxRound = m_tournament->numberOfRounds();
    } else {
        m_maxRound = *maxRound;
    }

    m_pairingsByPlayer = m_tournament->pairingsByPlayer(m_maxRound);
}

int State::lastRound() const
{
    return m_maxRound;
}

QList<Pairing *> State::pairings(Player *player) const
{
    return m_pairingsByPlayer.value(player);
}

double State::points(Player *player) const
{
    const auto pairings = m_pairingsByPlayer.value(player);

    const auto points = std::accumulate(pairings.constBegin(), pairings.constEnd(), 0., [&player](double acc, Pairing *pairing) {
        return acc + pairing->pointsOfPlayer(player);
    });

    return points;
}

double State::pointsForTiebreaks(Player *player) const
{
    double points = 0.;
    bool hadVUR = false;
    const auto pairings = m_pairingsByPlayer.value(player);
    for (auto pairing = pairings.rbegin(), rend = pairings.rend(); pairing != rend; ++pairing) {
        if (Pairing::isUnplayed((*pairing)->whiteResult())) {
            if (Pairing::isRequestedBye((*pairing)->whiteResult()) && (hadVUR || pairing == pairings.rbegin())) {
                points += .5;
                hadVUR = true;
            } else {
                points += (*pairing)->pointsOfPlayer(player);
                const auto result = (*pairing)->resultOfPlayer(player);
                if (pairing == pairings.rbegin()) {
                    hadVUR = Pairing::isVUR(result);
                } else {
                    hadVUR &= Pairing::isVUR(result);
                }
            }
        } else {
            points += (*pairing)->pointsOfPlayer(player);
            hadVUR = false;
        }
    }
    return points;
}
