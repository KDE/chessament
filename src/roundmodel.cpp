// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "roundmodel.h"

#include "libtournament/tournament.h"

RoundModel::RoundModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tournament *RoundModel::tournament()
{
    return m_tournament;
}

void RoundModel::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }

    beginResetModel();
    m_tournament = tournament;
    endResetModel();

    connect(m_tournament, &Tournament::numberOfRoundsChanged, [this]() {
        beginResetModel();
        endResetModel();
    });

    Q_EMIT tournamentChanged();
}

int RoundModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_tournament->numberOfRounds();
}

QVariant RoundModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto round = m_tournament->round(index.row() + 1);

    switch (role) {
    case RoundModel::RoundRole::DateTimeRole: {
        if (round == nullptr) {
            return QDateTime{};
        }
        return round->dateTime();
    }
    }

    return {};
}

bool RoundModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if (!index.isValid()) {
        return false;
    }

    if (const auto ok = m_tournament->ensureRoundExists(index.row() + 1); !ok) {
        return false;
    }
    const auto round = m_tournament->round(index.row() + 1);

    switch (role) {
    case RoundModel::RoundRole::DateTimeRole: {
        auto dateTime = value.toDateTime();
        auto time = dateTime.time();
        dateTime.setTime({time.hour(), time.minute(), 0});
        if (round->dateTime() == dateTime) {
            return false;
        }
        round->setDateTime(dateTime);
        if (const auto ok = m_tournament->saveRound(round); !ok) {
            return false;
        }
        break;
    }
    }

    Q_EMIT dataChanged(this->index(index.row()), this->index(index.row()));

    return true;
}

QHash<int, QByteArray> RoundModel::roleNames() const
{
    return {
        {RoundRole::DateTimeRole, "dateTime"},
    };
}
