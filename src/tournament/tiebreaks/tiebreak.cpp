// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include <QJsonObject>

#include "tiebreak.h"

using namespace Qt::Literals::StringLiterals;

QVariant Tiebreak::option(const QString &key, const QVariant &defaultValue)
{
    return m_options.value(key, defaultValue);
}

bool Tiebreak::isConfigurable()
{
    return false;
}

QList<QVariantMap> Tiebreak::options()
{
    return {};
}

void Tiebreak::setOptions(const QList<QVariantMap> &options)
{
    for (const auto &option : options) {
        m_options[option["id"_L1].toString()] = option["value"_L1];
    }
}

void Tiebreak::setOptions(const QVariantMap &options)
{
    for (const auto option : options.asKeyValueRange()) {
        m_options[option.first] = option.second;
    }
}

QJsonObject Tiebreak::toJson()
{
    return {
        {"id"_L1, id()},
        {"options"_L1, QJsonObject::fromVariantMap(m_options)},
    };
}
