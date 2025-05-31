// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#include "tournamentstate.h"

TournamentState::TournamentState(Tournament *tournament, int maxRound)
{
    m_tournament = tournament;
    m_maxRound = maxRound;
    m_pairingsByPlayer = m_tournament->getPairingsByPlayer(m_maxRound);
}

QList<Pairing *> TournamentState::getPairings(Player *player)
{
    return m_pairingsByPlayer.value(player);
}

double TournamentState::getPoints(Player *player)
{
    double points = 0.;
    for (const auto &pairing : m_pairingsByPlayer.value(player)) {
        points += pairing->getPointsOfPlayer(player);
    }
    return points;
}

double TournamentState::getPointsForTiebreaks(Player *player)
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
                points += (*pairing)->getPointsOfPlayer(player);
                hadVUR = ((*pairing)->whiteResult() == Pairing::PartialResult::LostForfeit && (*pairing)->blackResult() == Pairing::PartialResult::LostForfeit)
                    || ((*pairing)->whiteResult() == Pairing::PartialResult::LostForfeit && (*pairing)->whitePlayer() == player)
                    || ((*pairing)->blackResult() == Pairing::PartialResult::LostForfeit && (*pairing)->blackPlayer() == player);
            }
        } else {
            points += (*pairing)->getPointsOfPlayer(player);
            hadVUR = false;
        }
    }
    return points;
}
