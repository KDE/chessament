// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "state.h"
#include "trf.h"

class Tournament;
class QTextStream;

class TrfWriter : public QObject
{
    Q_OBJECT

private:
    explicit TrfWriter(Tournament *tournament, Trf::Options options, int maxRound = -1);

    void write(QTextStream *stream);

    void writeTournamentInformation(QTextStream *stream);
    void writePairingEngineInformation(QTextStream *stream);
    void writePlayers(QTextStream *stream);

    Tournament *m_tournament;
    Trf::Options m_options;

    State m_state;

    friend class Tournament;
};
