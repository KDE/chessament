// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class AverageRatingOfOpponents : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "aro"_L1;
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("Tiebreak", "Average Rating of Opponents");
    };

    [[nodiscard]] QString code() override
    {
        return "ARO"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
