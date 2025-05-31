// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include "tournament.h"

class TournamentState
{
public:
    explicit TournamentState(Tournament *tournament, int maxRound = -1);

    QList<Pairing *> getPairings(Player *player);
    double getPoints(Player *player);
    double getPointsForTiebreaks(Player *player);

private:
    Tournament *m_tournament;
    int m_maxRound;

    QHash<Player *, QList<Pairing *>> m_pairingsByPlayer;
};
