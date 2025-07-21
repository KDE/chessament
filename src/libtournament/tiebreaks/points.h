// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class Points : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "pts"_L1;
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("Game points", "Points");
    };

    [[nodiscard]] QString code() override
    {
        return "__pts"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
