// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "rep.h"

#include "state.h"

double RoundsElectedToPlay::calculate(Tournament *tournament, State state, QList<Player *> players, Player *player)
{
    Q_UNUSED(tournament)
    Q_UNUSED(players)

    const auto pairings = state.pairings(player);

    const auto result = std::ranges::count_if(pairings, [player](Pairing *const pairing) -> bool {
        return !Pairing::isVUR(pairing->resultOfPlayer(player));
    });

    return static_cast<double>(result);
}
