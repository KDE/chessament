// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "pairing.h"

/*!
 * \class Round
 * \inmodule libtournament
 * \inheaderfile tournament/round.h
 */
class Round : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY dateTimeChanged)

public:
    explicit Round() = default;

    /*!
     * \property Round::id
     *
     * This property holds the database ID of the round.
     */
    [[nodiscard]] int id() const;

    /*!
     * \property Round::number
     *
     * This property holds the round number.
     */
    [[nodiscard]] int number() const;

    /*!
     * \property Round::dateTime
     *
     * This property holds the date and time of the round.
     */
    [[nodiscard]] QDateTime dateTime() const;

    [[nodiscard]] QByteArray extraString() const;

    /*!
     * Returns the pairings of the round.
     */
    [[nodiscard]] QList<Pairing *> pairings() const;

    /*!
     * Returns the pairing in board \a board.
     */
    Pairing *pairing(int board);

    /*!
     * Adds \a pairing to the pairings of this round.
     */
    void addPairing(std::unique_ptr<Pairing> pairing);

    /*!
     * Removes the pairings that meet \a predicate.
     */
    void removePairings(std::function<bool(Pairing *)> predicate);

    [[nodiscard]] QJsonObject toJson() const;

public Q_SLOTS:
    void setId(int id);
    void setNumber(int number);
    void setDateTime(const QDateTime &dateTime);
    void setExtra(const QByteArray &extra);

Q_SIGNALS:
    void idChanged();
    void numberChanged();
    void dateTimeChanged();

private:
    int m_id{};
    int m_number{};
    QDateTime m_dateTime;
    QJsonObject m_extra;

    std::vector<std::unique_ptr<Pairing>> m_pairings;

    friend class Tournament;
};
