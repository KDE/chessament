// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "sb.h"

#include <KLocalizedString>
#include <QLocale>

using namespace Qt::Literals::StringLiterals;

class Buchholz : public SonnebornBergerBase
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
        return i18ncp("Buchholz N tiebreak, N is a number < 0", "Buchholz %1", "Buchholz %1", -cutLowest);
    };

    [[nodiscard]] QString code() override
    {
        const auto cutLowest = option("cut_lowest"_L1, 0).toInt();
        if (cutLowest == 0) {
            return "BH"_L1;
        }
        return "BH/C%1"_L1.arg(QString::number(cutLowest));
    }

    [[nodiscard]] double contribution(double pointsForResult, double value) const override;
};
