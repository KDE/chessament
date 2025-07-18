// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

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

    double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) override;
};

class Buchholz : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "bh"_L1;
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("Buchholz tiebreak", "Buchholz");
    };

    [[nodiscard]] QString code() override
    {
        return "BH"_L1;
    }

    [[nodiscard]] bool isConfigurable() override
    {
        return false; // TODO: implement modifiers
    }

    [[nodiscard]] QList<QVariantMap> options() override;

    double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) override;
};
