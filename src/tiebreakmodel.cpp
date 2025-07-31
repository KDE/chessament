// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tiebreakmodel.h"

#include "libtournament/tournament.h"

TiebreakModel::TiebreakModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tournament *TiebreakModel::tournament()
{
    return m_tournament;
}

void TiebreakModel::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }
    m_tournament = tournament;
    Q_EMIT tournamentChanged();
}

int TiebreakModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_tournament->tiebreaks().size();
}

QVariant TiebreakModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto tiebreak = m_tournament->tiebreaks()[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return tiebreak->name();
    case TiebreakModel::TiebreakRole::IsConfigurableRole:
        return tiebreak->isConfigurable();
    case TiebreakModel::TiebreakRole::OptionsRole:
        return QVariant::fromValue(tiebreak->options());
    }

    return {};
}

bool TiebreakModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if (!index.isValid()) {
        return false;
    }

    const auto tiebreak = m_tournament->tiebreaks()[index.row()];
    tiebreak->setOptions(value.value<QList<QVariantMap>>());

    m_tournament->saveTiebreaks();

    Q_EMIT dataChanged(this->index(index.row()), this->index(index.row()));

    return true;
}

QHash<int, QByteArray> TiebreakModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {TiebreakRole::IsConfigurableRole, "isConfigurable"},
        {TiebreakRole::OptionsRole, "options"},
    };
}

void TiebreakModel::addTiebreak(const QString &tiebreak)
{
    beginInsertRows({}, rowCount(), rowCount());

    auto tiebreaks = m_tournament->tiebreaks();
    tiebreaks << Tournament::tiebreak(tiebreak);
    m_tournament->setTiebreaks(tiebreaks);

    endInsertRows();
}

void TiebreakModel::remove(int row)
{
    beginRemoveRows({}, row, row);

    auto tiebreaks = m_tournament->tiebreaks();
    tiebreaks.removeAt(row);
    m_tournament->setTiebreaks(tiebreaks);

    endRemoveRows();
}

void TiebreakModel::moveUp(int row)
{
    Q_ASSERT(row > 0);

    auto tiebreaks = m_tournament->tiebreaks();
    std::swap(tiebreaks[row], tiebreaks[row - 1]);
    m_tournament->setTiebreaks(tiebreaks);

    Q_EMIT dataChanged(index(row - 1), index(row));
}

void TiebreakModel::moveDown(int row)
{
    Q_ASSERT(row + 1 < rowCount());

    auto tiebreaks = m_tournament->tiebreaks();
    std::swap(tiebreaks[row], tiebreaks[row + 1]);
    m_tournament->setTiebreaks(tiebreaks);

    Q_EMIT dataChanged(index(row), index(row + 1));
}
