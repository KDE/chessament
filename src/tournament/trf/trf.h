// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

namespace Trf
{
Q_NAMESPACE

constexpr QStringView DateFormat = u"yy/MM/dd";

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

};
