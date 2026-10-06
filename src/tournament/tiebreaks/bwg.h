// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class NumberOfGamesWonWithBlack : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "bwg"_L1;
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("Tiebreak", "Number of Games Won with Black (over the board)");
    };

    [[nodiscard]] QString code() override
    {
        return "BWG"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
