// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

#pragma once

#include <QList>
#include <QMap>
#include <QVariant>

class Tournament;
class Player;
class Pairing;
class State;

class Tiebreak
{
public:
    Tiebreak() = default;

    virtual ~Tiebreak() = default;

    [[nodiscard]] virtual QString id() = 0;

    [[nodiscard]] virtual QString shortName() = 0;

    [[nodiscard]] virtual QString name() = 0;

    [[nodiscard]] virtual QString code() = 0;

    [[nodiscard]] virtual bool isConfigurable();

    [[nodiscard]] virtual QList<QVariantMap> options();

    [[nodiscard]] QVariant option(const QString &key, const QVariant &defaultValue = {});

    void setOptions(const QList<QVariantMap> &options);

    void setOptions(const QVariantMap &options);

    virtual double calculate(Tournament *tournament, State state, QList<Player *> players, Player *player) = 0;

    QJsonObject toJson();

private:
    QVariantMap m_options;
};
