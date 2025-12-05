// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "round.h"

#include <algorithm>

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

QDateTime Round::dateTime() const
{
    return m_dateTime;
}

void Round::setDateTime(const QDateTime &dateTime)
{
    if (m_dateTime == dateTime) {
        return;
    }
    m_dateTime = dateTime;
    Q_EMIT dateTimeChanged();
}

QList<Pairing *> Round::pairings() const
{
    QList<Pairing *> result;
    result.reserve(static_cast<qsizetype>(m_pairings.size()));

    for (const auto &pairing : m_pairings) {
        result << pairing.get();
    }

    return result;
}

Pairing *Round::getPairing(int board)
{
    const auto it = std::ranges::find_if(m_pairings, [board](const std::unique_ptr<Pairing> &p) {
        return p->board() == board;
    });

    Q_ASSERT(it != m_pairings.end());

    return it->get();
}

void Round::addPairing(std::unique_ptr<Pairing> pairing)
{
    m_pairings.push_back(std::move(pairing));
}

void Round::removePairings(std::function<bool(Pairing *)> predicate)
{
    std::erase_if(m_pairings, [&predicate](const std::unique_ptr<Pairing> &pairing) {
        return predicate(pairing.get());
    });
}

#include "moc_round.cpp"
