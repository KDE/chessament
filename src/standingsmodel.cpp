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
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

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
            return QLocale().toString(value);
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
    return {
        {Qt::DisplayRole, "displayText"},
        {Qt::TextAlignmentRole, "textAlignment"},
        {StandingsModel::StandingRoles::EnableSort, "enableSort"},
    };
}

Qt::ItemFlags StandingsModel::flags(const QModelIndex &index) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant StandingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    Q_ASSERT(m_tournament != nullptr);

    if (role == Qt::DisplayRole) {
        switch (section) {
        case RankRole:
            return i18nc("@title:column Player Ranking", "Rank");
        case StartingRankRole:
            return i18nc("@title:column Player Starting Rank Number", "№");
        case TitleRole:
            return i18nc("@title:column Player Title", "Title");
        case NameRole:
            return i18nc("@title:column Player Name", "Name");
        default: {
            const auto tiebreak = m_tournament->tiebreaks().at(section - 4);
            if (!tiebreak->shortName().isNull()) {
                return tiebreak->shortName();
            }
            return tiebreak->code();
        }
        }
    }
    if (role == StandingsModel::StandingRoles::EnableSort) {
        return true;
    }

    return {};
}

void StandingsModel::setStandings(QList<Standing> standings)
{
    beginResetModel();
    m_standings = std::move(standings);
    endResetModel();
}

void StandingsModel::setTournament(Tournament *tournament)
{
    m_tournament = tournament;

    // We need to clear the current data as standings are calculated asynchronously
    // and we don't want to access data that has been deleted.
    setStandings({});
}

#include "moc_standingsmodel.cpp"
