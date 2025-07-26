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

public:
    explicit Round() = default;

    int id() const;
    int number() const;

    [[nodiscard]] QList<Pairing *> pairings() const;
    Pairing *getPairing(int board);
    void addPairing(std::unique_ptr<Pairing> pairing);
    void removePairings(std::function<bool(Pairing *)> predicate);

public Q_SLOTS:
    void setId(int id);
    void setNumber(int number);

Q_SIGNALS:
    void idChanged();
    void numberChanged();

private:
    int m_id;
    int m_number;

    std::vector<std::unique_ptr<Pairing>> m_pairings;

    friend class Tournament;
};
