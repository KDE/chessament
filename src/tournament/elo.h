// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "pairing.h"

#include <QObject>
#include <qqmlregistration.h>

struct EloChange {
    Q_GADGET
    QML_VALUE_TYPE(eloChange)

    Q_PROPERTY(double change MEMBER m_change CONSTANT)
    Q_PROPERTY(int ratingDifference MEMBER m_ratingDifference CONSTANT)
    Q_PROPERTY(double expectedScore MEMBER m_expectedScore CONSTANT)
    Q_PROPERTY(bool isDifferenceLimited MEMBER m_isDifferenceLimited CONSTANT)

public:
    explicit EloChange() = default;

    explicit EloChange(double change, int ratingDifference, double expectedScore, bool isDifferenceLimited);

private:
    double m_change;
    int m_ratingDifference;
    double m_expectedScore;
    bool m_isDifferenceLimited;
};

class Elo : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int rating READ rating WRITE setRating NOTIFY ratingChanged)
    Q_PROPERTY(int k READ k WRITE setK NOTIFY kChanged)

public:
    static std::array<int, 801> DiffToPd;

    explicit Elo() = default;

    explicit Elo(int rating, int k = 40);

    [[nodiscard]] int rating() const;

    [[nodiscard]] int k() const;

    [[nodiscard]] Q_INVOKABLE EloChange change(int opponentRating, Pairing::PartialResult result) const;

public Q_SLOTS:
    void setRating(int rating);

    void setK(int k);

Q_SIGNALS:
    void ratingChanged();

    void kChanged();

private:
    int m_rating{0};
    int m_k{40};
};
