// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class Progressive : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "ps"_L1;
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("Progressive Scores tiebreak", "Progressive");
    };

    [[nodiscard]] QString code() override
    {
        return "PS"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
