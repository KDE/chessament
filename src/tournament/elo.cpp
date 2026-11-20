// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "elo.h"

EloChange::EloChange(double change, int ratingDifference, double expectedScore, bool isDifferenceLimited)
    : m_change(change)
    , m_ratingDifference(ratingDifference)
    , m_expectedScore(expectedScore)
    , m_isDifferenceLimited(isDifferenceLimited)
{
}

double EloChange::change() const
{
    return m_change;
}

Elo::Elo(int rating, int k)
    : m_rating(rating)
    , m_k(k)
{
}

int Elo::rating() const
{
    return m_rating;
}

void Elo::setRating(int rating)
{
    if (m_rating == rating) {
        return;
    }
    m_rating = rating;
    Q_EMIT ratingChanged();
}

int Elo::k() const
{
    return m_k;
}

void Elo::setK(int k)
{
    if (m_k == k) {
        return;
    }
    m_k = k;
    Q_EMIT kChanged();
}

EloChange Elo::change(int opponentRating, Pairing::PartialResult result) const
{
    auto diff = opponentRating - m_rating;
    bool isDifferenceLimited{false};

    if (m_rating < 2650 && std::abs(diff) > 400) {
        diff = std::clamp(diff, -400, 400);
        isDifferenceLimited = true;
    }

    if (diff > 800 || diff < -800) {
        diff = std::clamp(diff, -800, 800);
    }

    int expectedScore;
    if (diff > 0) {
        expectedScore = 100 - Elo::DiffToPd.at(diff);
    } else {
        expectedScore = Elo::DiffToPd.at(-diff);
    }

    const auto score = static_cast<int>(100 * Pairing::pointsForResult(result));
    const auto delta = score - expectedScore;

    return EloChange{
        delta * m_k / 100.,
        diff,
        expectedScore / 100.,
        isDifferenceLimited,
    };
}
