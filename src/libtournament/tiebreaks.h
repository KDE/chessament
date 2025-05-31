// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include "tiebreak.h"

class Points : public Tiebreak
{
public:
    QString name() override
    {
        return QStringLiteral("Points");
    };

    double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) override;
};

class Buchholz : public Tiebreak
{
public:
    QString name() override
    {
        return QStringLiteral("Buchholz");
    };

    double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) override;
};
