// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include <QList>
#include <QMap>

class Tournament;
class Player;
class Pairing;
class TournamentState;

class Tiebreak
{
public:
    Tiebreak() = default;
    virtual ~Tiebreak() = default;

    virtual QString name() = 0;

    virtual double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) = 0;

private:
};

using Tiebreaks = QList<double>;
using PlayerTiebreaks = std::pair<Player *, Tiebreaks>;
