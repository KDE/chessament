// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

namespace Trf
{
Q_NAMESPACE

constexpr QStringView RoundDateFormat = u"yy/MM/dd";

enum class Field {
    Player = 1,
    TournamentName,
    City,
    Federation,
    StartDate,
    EndDate,
    NumberOfPlayers,
    NumberOfRatedPlayers,
    NumberOfTeams,
    TournamentType,
    ChiefArbiter,
    DeputyChiefArbiter,
    TimeControl,
    Calendar,
    Unknown,
};
Q_ENUM_NS(Field)

QString reportFieldString(Field field);

Field reportFieldForString(QStringView number);

/*!
 * \enum TrfWriter::TrfOption
 *
 * This enum type specifies the options for the Tournament Report File export.
 *
 * \value NumberOfRounds Adds the number of rounds of the tournament for pairing engines.
 * \value InitialColorWhite Adds an indication for pairing engines that the first player had the white pieces in the first round.
 * \value InitialColorBlack Adds an indication for pairing engines that the first player had the black pieces in the first round.
 *
 * \sa toTrf(), exportTrf()
 */
enum class Option {
    NumberOfRounds = 0x1,
    InitialColorWhite = 0x2,
    InitialColorBlack = 0x4,
};
Q_DECLARE_FLAGS(Options, Option)
Q_DECLARE_OPERATORS_FOR_FLAGS(Trf::Options)
};
