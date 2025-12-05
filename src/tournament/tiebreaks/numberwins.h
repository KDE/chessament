// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class NumberOfWins : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "win"_L1;
    }

    [[nodiscard]] QString shortName() override
    {
        return i18nc("Number of Wins tiebreak", "Wins");
    };

    [[nodiscard]] QString name() override
    {
        return i18nc("Number of Wins tiebreak", "Number of Wins");
    };

    [[nodiscard]] QString code() override
    {
        return "WIN"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
