// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "round.h"

Round::Round()
    : m_pairings(std::make_unique<std::vector<std::unique_ptr<Pairing>>>())
{
}

int Round::id() const
{
    return m_id;
}

void Round::setId(int id)
{
    if (m_id == id) {
        return;
    }
    m_id = id;
    Q_EMIT idChanged();
}

int Round::number() const
{
    return m_number;
}

void Round::setNumber(int number)
{
    if (m_number == number) {
        return;
    }
    m_number = number;
    Q_EMIT numberChanged();
}

std::vector<std::unique_ptr<Pairing>> *Round::pairings() const
{
    return m_pairings.get();
}

Pairing *Round::getPairing(int board)
{
    const auto it = std::find_if(m_pairings->begin(), m_pairings->end(), [board](const std::unique_ptr<Pairing> &p) {
        return p->board() == board;
    });

    Q_ASSERT(it != m_pairings->end());

    return it->get();
}

void Round::addPairing(std::unique_ptr<Pairing> pairing)
{
    m_pairings->push_back(std::move(pairing));
}

void Round::removePairings(std::function<bool(Pairing *)> predicate)
{
    std::erase_if(*m_pairings, [&predicate](const std::unique_ptr<Pairing> &pairing) {
        return predicate(pairing.get());
    });
}
