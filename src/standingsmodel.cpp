// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "standingsmodel.h"

#include "standing.h"

#include <QLocale>

StandingsModel::StandingsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int StandingsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_standings.size());
}

int StandingsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_standings.isEmpty()) {
        return 4;
    }

    return 4 + static_cast<int>(m_standings.at(0).values().size());
}

QVariant StandingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto standing = m_standings.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case RankRole:
            return standing.rank();
        case StartingRankRole:
            return standing.player()->startingRank();
        case TitleRole:
            return standing.player()->title();
        case NameRole:
            return standing.player()->name();
        default: {
            const auto value = standing.values().at(index.column() - 4);
            return QLocale::system().toString(value);
        }
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case NameRole:
            return Qt::AlignLeading;
        case TitleRole:
            return Qt::AlignCenter;
        default:
            return Qt::AlignTrailing;
        }
    }

    return {};
}

QHash<int, QByteArray> StandingsModel::roleNames() const
{
    return {{Qt::DisplayRole, "display"}, {Qt::TextAlignmentRole, "textAlignment"}};
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

    Q_ASSERT(m_tournament != nullptr);

    switch (section) {
    case RankRole:
        return i18nc("@title:column Player Ranking", "Rank");
    case StartingRankRole:
        return i18nc("@title:column Player Starting Rank Number", "№");
    case TitleRole:
        return i18nc("@title:column Player Title", "Title");
    case NameRole:
        return i18nc("@title:column Player Name", "Name");
    default:
        return m_tournament->tiebreaks().at(section - 4)->shortName();
    }
}

void StandingsModel::setStandings(QList<Standing> standings)
{
    auto rowDiff = standings.size() - m_standings.size();

    auto newColumns = 4 + (standings.isEmpty() ? 0 : standings[0].values().size());
    auto prevColumns = 4 + (m_standings.isEmpty() ? 0 : m_standings[0].values().size());
    auto columnDiff = newColumns - prevColumns;

    if (rowDiff > 0) {
        beginInsertRows({}, static_cast<int>(m_standings.size()), static_cast<int>(standings.size()) - 1);
    } else if (rowDiff < 0) {
        beginRemoveRows({}, static_cast<int>(standings.size()), static_cast<int>(m_standings.size()) - 1);
    }
    if (columnDiff > 0) {
        beginInsertColumns({}, static_cast<int>(prevColumns), static_cast<int>(newColumns) - 1);
    } else if (columnDiff < 0) {
        beginRemoveColumns({}, static_cast<int>(newColumns), static_cast<int>(prevColumns) - 1);
    }

    m_standings = std::move(standings);

    if (rowDiff > 0) {
        endInsertRows();
    } else if (rowDiff < 0) {
        endRemoveRows();
    }
    if (columnDiff > 0) {
        endInsertColumns();
    } else if (columnDiff < 0) {
        endRemoveColumns();
    }

    Q_EMIT headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
    Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void StandingsModel::setTournament(Tournament *tournament)
{
    m_tournament = tournament;

    // We need to clear the current data as standings are calculated asynchronously
    // and we don't want to access data that has been deleted.
    setStandings({});
}

#include "moc_standingsmodel.cpp"
