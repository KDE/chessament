// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QString>

#include "pairing.h"

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

    [[nodiscard]] int id() const;
    [[nodiscard]] int number() const;
    [[nodiscard]] QDateTime dateTime() const;

    [[nodiscard]] QList<Pairing *> pairings() const;
    Pairing *getPairing(int board);
    void addPairing(std::unique_ptr<Pairing> pairing);
    void removePairings(std::function<bool(Pairing *)> predicate);

public Q_SLOTS:
    void setId(int id);
    void setNumber(int number);
    void setDateTime(const QDateTime &dateTime);

Q_SIGNALS:
    void idChanged();
    void numberChanged();
    void dateTimeChanged();

private:
    int m_id;
    int m_number;
    QDateTime m_dateTime;

    std::vector<std::unique_ptr<Pairing>> m_pairings;

    friend class Tournament;
};
