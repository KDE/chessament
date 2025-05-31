// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCoroProcess>
#include <QList>
#include <QObject>

#include <expected>

class Tournament;

#include "pairing.h"
#include "tournament.h"

class PairingEngine : public QObject
{
    Q_OBJECT

public:
    explicit PairingEngine();

    QCoro::Task<std::expected<QList<Pairing *>, QString>> pair(int round, Tournament *tournament);
};
