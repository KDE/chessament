// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <vector>

class Player;

class Standing
{
public:
    explicit Standing(Player *player, std::vector<double> values);

    [[nodiscard]] Player *player() const;
    [[nodiscard]] std::vector<double> values() const;

    void addValue(double value);

private:
    Player *m_player;
    std::vector<double> m_values;
};
