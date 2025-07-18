// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include "tiebreak.h"

using namespace Qt::Literals::StringLiterals;

class Points : public Tiebreak
{
public:
    [[nodiscard]] QString name() const override
    {
        return "Points"_L1;
    };

    QString code() override
    {
        return "__pts"_L1;
    }

    double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) override;
};

class Buchholz : public Tiebreak
{
public:
    [[nodiscard]] QString name() const override
    {
        return "Buchholz"_L1;
    };

    QString code() override
    {
        return "BH"_L1;
    }

    double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) override;
};
