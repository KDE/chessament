// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "points.h"
#include "state.h"

double Points::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    return state.getPoints(player);
}
