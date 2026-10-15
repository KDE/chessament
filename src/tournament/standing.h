// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <vector>

class Player;

/*!
 * \class Standing
 * \inmodule libtournament
 * \inheaderfile tournament/standing.h
 */
class Standing
{
public:
    explicit Standing(Player *player, std::vector<double> values);

    /*!
     * Returns the rank of the player.
     */
    [[nodiscard]] int rank() const;

    /*!
     * Returns the player associated with this standing.
     */
    [[nodiscard]] Player *player() const;

    /*!
     * Returns the tiebreak values of this player.
     */
    [[nodiscard]] std::vector<double> values() const;

    void setRank(int rank);
    void addValue(double value);

    bool operator<(const Standing &other) const;

private:
    int m_rank = 1;
    Player *m_player;
    std::vector<double> m_values;
};
