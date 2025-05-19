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

    Q_PROPERTY(QList<Pairing *> pairings READ pairings WRITE setPairings NOTIFY pairingsChanged)

public:
    explicit Round();

    int id();
    int number();

    QList<Pairing *> pairings();
    Pairing *getPairing(int board);
    void addPairing(Pairing *pairing);
    void removePairings(std::function<bool(Pairing *)> predicate);

public Q_SLOTS:
    void setId(int id);
    void setNumber(int number);
    void setPairings(QList<Pairing *> pairings);

Q_SIGNALS:
    void idChanged();
    void numberChanged();
    void pairingsChanged();

private:
    int m_id;
    int m_number;

    QList<Pairing *> m_pairings;
};
