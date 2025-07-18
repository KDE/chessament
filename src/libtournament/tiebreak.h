// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include <QList>
#include <QMap>
#include <QVariant>

class Tournament;
class Player;
class Pairing;
class TournamentState;

class Tiebreak
{
public:
    Tiebreak() = default;

    virtual ~Tiebreak() = default;

    [[nodiscard]] virtual QString name() const = 0;

    virtual QString code() = 0;

    [[nodiscard]] QMap<QString, QVariantMap> availableOptions() const;

    [[nodiscard]] QVariant option(const QString &key, const QVariant &defaultValue = {}) const;

    virtual double calculate(Tournament *tournament, TournamentState state, QList<Player *> players, Player *player) = 0;

private:
    QVariantMap m_options;
};

using Tiebreaks = QList<double>;
using PlayerTiebreaks = std::pair<Player *, Tiebreaks>;
