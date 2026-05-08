// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timecontrolmodel.h"

#include "tournament/tournament.h"

TimeControlModel::TimeControlModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tournament *TimeControlModel::tournament()
{
    return m_tournament;
}

void TimeControlModel::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }
    m_tournament = tournament;
    Q_EMIT tournamentChanged();
}

int TimeControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_tournament->timeControl().periods().size());
}

QVariant TimeControlModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto &timeControl = m_tournament->timeControl();
    const auto period = timeControl.periods()[index.row()];

    switch (role) {
    case TimeControlModel::Roles::Moves:
        return period.moves().value_or(0);
    case TimeControlModel::Roles::Time:
        return period.time();
    case TimeControlModel::Roles::Increment:
        return period.increment();
    }

    return {};
}

bool TimeControlModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    auto &timeControl = m_tournament->timeControl();
    const auto period = timeControl.periods()[index.row()];
    TimeControlPeriod newPeriod;

    switch (role) {
    case TimeControlModel::Roles::Moves:
        newPeriod = TimeControlPeriod{value.toInt(), period.time(), period.increment()};
        break;
    case TimeControlModel::Roles::Time:
        newPeriod = TimeControlPeriod{period.moves(), value.toInt(), period.increment()};
        break;
    case TimeControlModel::Roles::Increment:
        newPeriod = TimeControlPeriod{period.moves(), period.time(), value.toInt()};
        break;
    default:
        return false;
    }

    timeControl.setPeriod(index.row(), newPeriod);
    m_tournament->saveTimeControl();

    Q_EMIT dataChanged(this->index(index.row()), this->index(index.row()));

    return true;
}

QHash<int, QByteArray> TimeControlModel::roleNames() const
{
    return {
        {TimeControlModel::Roles::Moves, "moves"},
        {TimeControlModel::Roles::Time, "time"},
        {TimeControlModel::Roles::Increment, "increment"},
    };
}

void TimeControlModel::addPeriod()
{
    beginInsertRows({}, rowCount(), rowCount());
    m_tournament->timeControl().addPeriod();
    endInsertRows();

    m_tournament->saveTimeControl();
}

void TimeControlModel::deletePeriod(int row)
{
    beginRemoveRows({}, row, row);
    m_tournament->timeControl().removePeriod(row);
    endRemoveRows();

    m_tournament->saveTimeControl();
}

#include "moc_timecontrolmodel.cpp"
