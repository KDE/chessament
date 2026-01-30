// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "writer.h"

#include <QCoreApplication>

#include "reader.h"
#include "state.h"
#include "tournament.h"
#include "trf.h"

constexpr auto space = " "_L1;
constexpr auto newLine = '\n'_L1;

TrfWriter::TrfWriter(Tournament *tournament, Trf::Options options, int maxRound)
    : m_tournament(tournament)
    , m_options(options)
    , m_state(m_tournament->getState(maxRound))
{
}

void TrfWriter::write(QTextStream &stream)
{
    writeTournamentInformation(stream);
    writePairingEngineInformation(stream);
    writePlayers(stream);
}

void TrfWriter::writeTournamentInformation(QTextStream &stream)
{
    stream << Trf::reportFieldString(Trf::Field::TournamentName) << space << m_tournament->name() << newLine;
    stream << Trf::reportFieldString(Trf::Field::City) << space << m_tournament->city() << newLine;
    stream << Trf::reportFieldString(Trf::Field::Federation) << space << m_tournament->federation() << newLine;
    stream << Trf::reportFieldString(Trf::Field::NumberOfPlayers) << space << m_tournament->numberOfPlayers() << newLine;
    stream << Trf::reportFieldString(Trf::Field::NumberOfRatedPlayers) << space << m_tournament->numberOfRatedPlayers() << newLine;
    stream << Trf::reportFieldString(Trf::Field::ChiefArbiter) << space << m_tournament->chiefArbiter() << newLine;
    stream << Trf::reportFieldString(Trf::Field::TimeControl) << space << m_tournament->timeControl() << newLine;
    stream << Trf::reportFieldString(Trf::Field::ProgramName) << space << "Chessament %1"_L1.arg(QCoreApplication::applicationVersion()) << newLine;

    stream << Trf::reportFieldString(Trf::Field::Calendar) << QString(space).repeated(86);
    for (size_t i = 0; i < m_state.lastRound(); ++i) {
        QString date;
        if (i < m_tournament->m_rounds.size() && m_tournament->m_rounds[i]->dateTime().isValid()) {
            date = m_tournament->m_rounds[i]->dateTime().toString(Trf::RoundDateFormat);
        } else {
            date = "        "_L1;
        }
        stream << "  "_L1 << date;
    }
    stream << newLine;
}

void TrfWriter::writePairingEngineInformation(QTextStream &stream)
{
    if (m_options.testFlag(Trf::Option::NumberOfRounds)) {
        stream << "XXR "_L1 + QString::number(m_tournament->numberOfRounds()) << newLine;
    }

    if (m_options.testFlag(Trf::Option::InitialColorWhite)) {
        stream << "XXC white1\n"_L1;
    } else if (m_options.testFlag(Trf::Option::InitialColorBlack)) {
        stream << "XXC black1\n"_L1;
    }
}

void TrfWriter::writePlayers(QTextStream &stream)
{
    const auto standings = m_tournament->getStandings(m_state);
    auto players = m_tournament->players();

    std::ranges::sort(players, Player::SortByStartingRank);

    for (const auto &player : std::as_const(players)) {
        const auto standing = std::find_if(standings.constBegin(), standings.constEnd(), [&player](const Standing &s) -> bool {
            return s.player() == player;
        });
        const auto rank = std::distance(standings.constBegin(), standing) + 1;
        const auto result = player->toTrf(m_state.getPoints(player), rank);

        stream << result.c_str();

        const auto pairings = m_state.getPairings(player);
        for (int i = 0; i < m_state.lastRound(); i++) {
            if (i < pairings.size()) {
                stream << pairings.value(i)->toTrf(player);
            } else {
                stream << "          "_L1;
            }
        }

        stream << newLine;
    }
}
