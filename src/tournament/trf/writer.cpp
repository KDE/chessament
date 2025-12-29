// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "writer.h"

#include "reader.h"
#include "state.h"
#include "tournament.h"
#include "trf.h"

TrfWriter::TrfWriter(Tournament *tournament, Options options)
    : m_tournament(tournament)
    , m_options(options)
{
}

void TrfWriter::write(QTextStream *stream, int maxRound)
{
    auto state = m_tournament->getState(maxRound);
    auto standings = m_tournament->getStandings(state);

    const auto space = u" "_s;
    const auto newLine = QLatin1Char('\n');

    *stream << reportFieldString(Trf::Field::TournamentName) << space << m_tournament->name() << newLine;
    *stream << reportFieldString(Trf::Field::City) << space << m_tournament->city() << newLine;
    *stream << reportFieldString(Trf::Field::Federation) << space << m_tournament->federation() << newLine;
    *stream << reportFieldString(Trf::Field::NumberOfPlayers) << space << m_tournament->numberOfPlayers() << newLine;
    *stream << reportFieldString(Trf::Field::NumberOfRatedPlayers) << space << m_tournament->numberOfRatedPlayers() << newLine;
    *stream << reportFieldString(Trf::Field::ChiefArbiter) << space << m_tournament->chiefArbiter() << newLine;
    *stream << reportFieldString(Trf::Field::TimeControl) << space << m_tournament->timeControl() << newLine;

    if (m_options.testFlag(Option::NumberOfRounds)) {
        *stream << u"XXR "_s + QString::number(m_tournament->numberOfRounds()) << newLine;
    }

    if (m_options.testFlag(Option::InitialColorWhite)) {
        *stream << u"XXC white1\n"_s;
    } else if (m_options.testFlag(Option::InitialColorBlack)) {
        *stream << u"XXC black1\n"_s;
    }

    *stream << reportFieldString(Trf::Field::Calendar) << space.repeated(86);
    for (size_t i = 0; i < state.lastRound(); ++i) {
        QString date;
        if (i < m_tournament->m_rounds.size() && m_tournament->m_rounds[i]->dateTime().isValid()) {
            date = m_tournament->m_rounds[i]->dateTime().toString(Trf::DateFormat);
        } else {
            date = "        "_L1;
        }
        *stream << "  "_L1 << date;
    }
    *stream << newLine;

    for (const auto &player : std::as_const(m_tournament->m_players)) {
        const auto standing = std::find_if(standings.constBegin(), standings.constEnd(), [&player](const Standing &s) -> bool {
            return s.player() == player.get();
        });
        const auto rank = std::distance(standings.constBegin(), standing) + 1;
        const auto result = player->toTrf(state.getPoints(player.get()), rank);

        *stream << result.c_str();

        auto pairings = state.getPairings(player.get());
        for (int i = 0; i < state.lastRound(); i++) {
            if (i < pairings.size()) {
                *stream << pairings.value(i)->toTrf(player.get());
            } else {
                *stream << u"          "_s;
            }
        }

        *stream << newLine;
    }
}
