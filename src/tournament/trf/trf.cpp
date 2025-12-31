// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "trf.h"

using namespace Qt::StringLiterals;

namespace Trf
{

QString reportFieldString(Trf::Field field)
{
    switch (field) {
    case Trf::Field::Player:
        return QStringLiteral("001");
    case Trf::Field::TournamentName:
        return QStringLiteral("012");
    case Trf::Field::City:
        return QStringLiteral("022");
    case Trf::Field::Federation:
        return QStringLiteral("032");
    case Trf::Field::StartDate:
        return QStringLiteral("042");
    case Trf::Field::EndDate:
        return QStringLiteral("052");
    case Trf::Field::NumberOfPlayers:
        return QStringLiteral("062");
    case Trf::Field::NumberOfRatedPlayers:
        return QStringLiteral("072");
    case Trf::Field::NumberOfTeams:
        return QStringLiteral("082");
    case Trf::Field::TournamentType:
        return QStringLiteral("092");
    case Trf::Field::ChiefArbiter:
        return QStringLiteral("102");
    case Trf::Field::DeputyChiefArbiter:
        return QStringLiteral("112");
    case Trf::Field::TimeControl:
        return QStringLiteral("122");
    case Trf::Field::Calendar:
        return QStringLiteral("132");
    case Trf::Field::ProgramName:
        return "182"_L1;
    case Trf::Field::Unknown:
        return {};
    }
    Q_UNREACHABLE();
};

Trf::Field reportFieldForString(QStringView number)
{
    if (number == QStringLiteral("001")) {
        return Trf::Field::Player;
    } else if (number == QStringLiteral("012")) {
        return Trf::Field::TournamentName;
    } else if (number == QStringLiteral("022")) {
        return Trf::Field::City;
    } else if (number == QStringLiteral("032")) {
        return Trf::Field::Federation;
    } else if (number == QStringLiteral("042")) {
        return Trf::Field::StartDate;
    } else if (number == QStringLiteral("052")) {
        return Trf::Field::EndDate;
    } else if (number == QStringLiteral("062")) {
        return Trf::Field::NumberOfPlayers;
    } else if (number == QStringLiteral("072")) {
        return Trf::Field::NumberOfRatedPlayers;
    } else if (number == QStringLiteral("082")) {
        return Trf::Field::NumberOfTeams;
    } else if (number == QStringLiteral("092")) {
        return Trf::Field::TournamentType;
    } else if (number == QStringLiteral("102")) {
        return Trf::Field::ChiefArbiter;
    } else if (number == QStringLiteral("112")) {
        return Trf::Field::DeputyChiefArbiter;
    } else if (number == QStringLiteral("122")) {
        return Trf::Field::TimeControl;
    } else if (number == QStringLiteral("132")) {
        return Trf::Field::Calendar;
    }
    return Trf::Field::Unknown;
}

};
