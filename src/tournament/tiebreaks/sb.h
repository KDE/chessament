// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "tiebreak.h"

#include <KLocalizedString>
#include <QLocale>

using namespace Qt::Literals::StringLiterals;

class SonnebornBergerBase : public Tiebreak
{
public:
    [[nodiscard]] bool isConfigurable() override
    {
        return true;
    }

    [[nodiscard]] QList<QVariantMap> options() override;

    std::expected<void, QString> setTrfOptions(const QList<QString> &options) override;

    double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) override;

    [[nodiscard]] virtual double contribution(double pointsForResult, double value) const = 0;

private:
    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    struct round {
        double value; // The opponent's points
        double contribution; // The actual addend
        bool isVur;

        bool operator==(round other) const
        {
            return value == other.value && contribution == other.contribution && isVur == other.isVur;
        }
    };
    // NOLINTEND(misc-non-private-member-variables-in-classes)

    void processCuts(QList<round> &contributions);
};

class SonnebornBerger : public SonnebornBergerBase
{
public:
    [[nodiscard]] QString id() override
    {
        return "sb"_L1;
    }

    [[nodiscard]] QString name() override
    {
        const auto cutLowest = option("cut_lowest"_L1, 0).toInt();
        if (cutLowest == 0) {
            return i18nc("Tiebreak", "Sonneborn-Berger");
        }
        return i18ncp("Tiebreak modifier, N is a number < 0", "Sonneborn-Berger %1", "Sonneborn-Berger %1", -cutLowest);
    };

    [[nodiscard]] QString code() override
    {
        const auto cutLowest = option("cut_lowest"_L1, 0).toInt();
        if (cutLowest == 0) {
            return "SB"_L1;
        }
        return "SB/C%1"_L1.arg(QString::number(cutLowest));
    }

    [[nodiscard]] double contribution(double pointsForResult, double value) const override;
};
