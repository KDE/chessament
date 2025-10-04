// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "standing.h"

Standing::Standing(Player *player, std::vector<double> values)
    : m_player(player)
    , m_values(std::move(values))
{
}

Player *Standing::player() const
{
    return m_player;
}

std::vector<double> Standing::values() const
{
    return m_values;
}

void Standing::addValue(double value)
{
    m_values.push_back(value);
}
