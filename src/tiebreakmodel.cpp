// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tiebreakmodel.h"

#include "tournament/tournament.h"

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
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto &tiebreak = m_tournament->tiebreaks().at(index.row());

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
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    auto tiebreak = m_tournament->tiebreaks().at(index.row());

    switch (role) {
    case TiebreakModel::TiebreakRole::OptionsRole: {
        tiebreak->setOptions(value.value<QList<QVariantMap>>());
        break;
    }
    default:
        return false;
    }

    m_tournament->tiebreaks().setTiebreak(index.row(), std::move(tiebreak));
    m_tournament->saveTiebreaks();

    Q_EMIT dataChanged(this->index(index.row()), this->index(index.row()));

    return true;
}

QHash<int, QByteArray> TiebreakModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "name"},
        {TiebreakRole::IsConfigurableRole, "isConfigurable"},
        {TiebreakRole::OptionsRole, "options"},
    };
}

void TiebreakModel::addTiebreak(const QString &tiebreak)
{
    beginInsertRows({}, rowCount(), rowCount());

    auto &tiebreaks = m_tournament->tiebreaks();
    tiebreaks.addTiebreak(Tiebreaks::tiebreak(tiebreak));
    m_tournament->saveTiebreaks();

    endInsertRows();
}

void TiebreakModel::remove(int row)
{
    beginRemoveRows({}, row, row);

    auto &tiebreaks = m_tournament->tiebreaks();
    tiebreaks.removeTiebreak(row);
    m_tournament->saveTiebreaks();

    endRemoveRows();
}

void TiebreakModel::moveUp(int row)
{
    Q_ASSERT(row > 0);

    m_tournament->tiebreaks().swapTiebreaks(row, row - 1);
    m_tournament->saveTiebreaks();

    Q_EMIT dataChanged(index(row - 1), index(row));
}

void TiebreakModel::moveDown(int row)
{
    Q_ASSERT(row + 1 < rowCount());

    m_tournament->tiebreaks().swapTiebreaks(row, row + 1);
    m_tournament->saveTiebreaks();

    Q_EMIT dataChanged(index(row), index(row + 1));
}

#include "moc_tiebreakmodel.cpp"
