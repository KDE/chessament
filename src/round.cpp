// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "round.h"

Round::Round()
    : QObject()
{
}

int Round::id()
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

int Round::number()
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

QList<Pairing *> Round::pairings()
{
    return m_pairings;
}

void Round::setPairings(QList<Pairing *> pairings)
{
    if (m_pairings == pairings) {
        return;
    }
    m_pairings = pairings;
    Q_EMIT pairingsChanged();
}

Pairing *Round::getPairing(int board)
{
    const auto it = std::find_if(m_pairings.begin(), m_pairings.end(), [board](Pairing *p) {
        return p->board() == board;
    });

    Q_ASSERT(it != m_pairings.end());

    return *it;
}

void Round::addPairing(Pairing *pairing)
{
    m_pairings.append(pairing);
}

void Round::removePairings(std::function<bool(Pairing *)> predicate)
{
    m_pairings.removeIf(predicate);
}
