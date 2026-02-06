// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "state.h"

State::State(Tournament *tournament, int maxRound)
    : m_tournament(tournament)
{
    if (maxRound < 0) {
        m_maxRound = m_tournament->numberOfRounds();
    } else {
        m_maxRound = maxRound;
    }

    m_pairingsByPlayer = m_tournament->pairingsByPlayer(m_maxRound);
}

size_t State::lastRound() const
{
    return m_maxRound;
}

QList<Pairing *> State::pairings(Player *player) const
{
    return m_pairingsByPlayer.value(player);
}

double State::points(Player *player) const
{
    double points = 0.;
    const auto pairings = m_pairingsByPlayer.value(player);
    for (const auto &pairing : pairings) {
        points += pairing->pointsOfPlayer(player);
    }
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
