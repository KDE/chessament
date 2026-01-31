// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class NumberOfGamesPlayedWithBlack : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "bpg"_L1;
    }

    QString shortName() override
    {
        return i18nc("Number of Games Played with Black tiebreak", "Played With Black");
    };

    [[nodiscard]] QString name() override
    {
        return i18nc("Number of Games Played with Black tiebreak", "Number of Games Played with Black");
    };

    [[nodiscard]] QString code() override
    {
        return "BPG"_L1;
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
