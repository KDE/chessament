// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class NumberOfGamesWon : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "won"_L1;
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("Number of Games Won (over the board) tiebreak. Different from the 'Number of Wins' tiebreak, which includes games won by forfeit.",
                     "Number of Games Won (over the board)");
    };

    [[nodiscard]] QString code() override
    {
        return "WON"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
