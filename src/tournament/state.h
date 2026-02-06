// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tournament.h"

class State
{
public:
    explicit State(Tournament *tournament, int maxRound = -1);

    [[nodiscard]] size_t lastRound() const;

    QList<Pairing *> pairings(Player *player) const;
    double points(Player *player) const;
    double pointsForTiebreaks(Player *player) const;

private:
    Tournament *m_tournament;
    int m_maxRound;

    QHash<Player *, QList<Pairing *>> m_pairingsByPlayer;
};
