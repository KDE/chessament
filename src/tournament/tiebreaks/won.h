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

    QString shortName() override
    {
        return i18nc("Number of Games Won tiebreak", "Games Won");
    };

    [[nodiscard]] QString name() override
    {
        return i18nc("Number of Games Won tiebreak", "Number of Games Won");
    };

    [[nodiscard]] QString code() override
    {
        return "WON"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
