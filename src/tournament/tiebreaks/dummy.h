// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "state.h"
#include "tiebreak.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class DummyTiebreak : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return option(u"id"_s).toString();
    }

    [[nodiscard]] QString name() override
    {
        return i18nc("@info", "Unsupported tiebreak");
    };

    [[nodiscard]] QString code() override
    {
        return {};
    }

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override
    {
        Q_UNUSED(tournament)
        Q_UNUSED(state)
        Q_UNUSED(players)
        Q_UNUSED(player)

        return 0;
    }
};
