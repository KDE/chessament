// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>
#include <QLocale>

using namespace Qt::Literals::StringLiterals;

class Buchholz : public Tiebreak
{
public:
    [[nodiscard]] QString id() override
    {
        return "bh"_L1;
    }

    [[nodiscard]] QString name() override
    {
        const auto cutLowest = option("cut_lowest"_L1, 0).toInt();
        if (cutLowest == 0) {
            return i18nc("Buchholz tiebreak", "Buchholz");
        }
        const auto cutText = QLocale::system().toString(-cutLowest);
        return i18nc("Buchholz N tiebreak, N is a number < 0", "Buchholz %1", cutText);
    };

    [[nodiscard]] QString code() override
    {
        return "BH"_L1;
    }

    [[nodiscard]] bool isConfigurable() override
    {
        return true;
    }

    [[nodiscard]] QList<QVariantMap> options() override;

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;
};
