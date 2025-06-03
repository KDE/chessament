// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "standingsmodel.h"

#include <limits>

StandingsModel::StandingsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int StandingsModel::rowCount(const QModelIndex &) const
{
    return m_standings.size();
}

int StandingsModel::columnCount(const QModelIndex &) const
{
    return 4 + m_tournament->tiebreaks().size();
}

QVariant StandingsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    auto standing = m_standings.at(index.row());

    switch (index.column()) {
    case RankRole:
        return index.row() + 1;
    case StartingRankRole:
        return standing.first->startingRank();
    case TitleRole:
        return Player::titleString(standing.first->title());
    case NameRole:
        return standing.first->fullName();
    default:
        return standing.second.at(index.column() - 4);
    }
}

QHash<int, QByteArray> StandingsModel::roleNames() const
{
    return {{Qt::DisplayRole, "displayName"}};
}

Qt::ItemFlags StandingsModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant StandingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    switch (section) {
    case RankRole:
        return i18nc("@title:colunm", "Rank");
    case StartingRankRole:
        return i18nc("@title:column", "No");
    case TitleRole:
        return i18nc("@title:column", "Title");
    case NameRole:
        return i18nc("@title:column", "Name");
    default:
        return m_tournament->tiebreaks().at(section - 4)->name();
    }
}

void StandingsModel::setTournament(Tournament *tournament)
{
    beginResetModel();
    m_tournament = tournament;
    m_standings = m_tournament->getStandings();
    endResetModel();
}
