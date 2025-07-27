// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "playersmodel.h"

#include <KLocalizedString>

#include "libtournament/tournament.h"

PlayersModel::PlayersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PlayersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_players.size());
}

int PlayersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_columns.size());
}

QVariant PlayersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto player = m_players.at(index.row());
    int column = m_columns.at(index.column());

    if (role == Qt::DisplayRole) {
        switch (column) {
        case PlayersModel::Columns::StartingRank:
            return player->startingRank();
        case PlayersModel::Columns::Title:
            return Player::titleString(player->title());
        case PlayersModel::Columns::Name:
            return player->name();
        case PlayersModel::Columns::Rating:
            return QLocale::system().toString(player->rating());
        case PlayersModel::Columns::NationalRating:
            return QLocale::system().toString(player->nationalRating());
        case PlayersModel::Columns::PlayerId:
            return player->playerId();
        case PlayersModel::Columns::BirthDate:
            return player->birthDate();
        case PlayersModel::Columns::Federation:
            return player->federation();
        case PlayersModel::Columns::Origin:
            return player->origin();
        case PlayersModel::Columns::Gender:
            return player->sex();
        }
    } else if (role == Qt::EditRole) {
        switch (column) {
        case PlayersModel::Columns::StartingRank:
            return player->startingRank();
        case PlayersModel::Columns::Title:
            return Player::titleString(player->title());
        case PlayersModel::Columns::Name:
            return player->name();
        case PlayersModel::Columns::Rating:
            return player->rating();
        case PlayersModel::Columns::NationalRating:
            return player->nationalRating();
        case PlayersModel::Columns::PlayerId:
            return player->playerId();
        case PlayersModel::Columns::BirthDate:
            return player->birthDate();
        case PlayersModel::Columns::Federation:
            return player->federation();
        case PlayersModel::Columns::Origin:
            return player->origin();
        case PlayersModel::Columns::Gender:
            return player->sex();
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (column) {
        case PlayersModel::Columns::StartingRank:
        case PlayersModel::Columns::Rating:
        case PlayersModel::Columns::NationalRating:
        case PlayersModel::Columns::PlayerId:
            return Qt::AlignTrailing;
        default:
            return Qt::AlignLeading;
        }
    }

    return {};
}

bool PlayersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT(role == Qt::EditRole);

    if (!index.isValid()) {
        return false;
    }

    const auto player = m_players.at(index.row());
    const auto field = static_cast<Columns>(m_columns.at(index.column()));

    switch (field) {
    case PlayersModel::Columns::StartingRank: {
        auto rank = value.toInt();
        if (rank == player->startingRank() || rank <= 0) {
            return false;
        }
        m_tournament->changePlayerStartingRank(player, rank);
        break;
    }
    case PlayersModel::Columns::Title:
        player->setTitle(Player::titleForString(value.toString()));
        break;
    case PlayersModel::Columns::Name:
        player->setName(value.toString());
        break;
    case PlayersModel::Columns::Rating:
        player->setRating(value.toInt());
        break;
    case PlayersModel::Columns::NationalRating:
        player->setNationalRating(value.toInt());
        break;
    case PlayersModel::Columns::PlayerId:
        player->setPlayerId(value.toString());
        break;
    case PlayersModel::Columns::BirthDate:
        player->setBirthDate(value.toString());
        break;
    case PlayersModel::Columns::Federation:
        player->setFederation(value.toString());
        break;
    case PlayersModel::Columns::Origin:
        player->setOrigin(value.toString());
        break;
    case PlayersModel::Columns::Gender:
        player->setSex(value.toString());
        break;
    }

    Q_EMIT dataChanged(index, index, {});
    Q_EMIT playerChanged(player, field);

    return true;
}

QHash<int, QByteArray> PlayersModel::roleNames() const
{
    return {{Qt::DisplayRole, "display"}, {Qt::EditRole, "edit"}, {Qt::TextAlignmentRole, "textAlignment"}};
}

Qt::ItemFlags PlayersModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant PlayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    switch (m_columns.at(section)) {
    case PlayersModel::Columns::StartingRank:
        return i18nc("@title:column Player Starting Rank Number", "№");
    case PlayersModel::Columns::Title:
        return i18nc("@title:column Player Title", "Title");
    case PlayersModel::Columns::Name:
        return i18nc("@title:column Player Name", "Name");
    case PlayersModel::Columns::Rating:
        return i18nc("@title:column Player Rating", "Rating");
    case PlayersModel::Columns::NationalRating:
        return i18nc("@title:column Player National Rating", "National Rating");
    case PlayersModel::Columns::PlayerId:
        return i18nc("@title:column", "Player ID");
    case PlayersModel::Columns::BirthDate:
        return i18nc("@title:column", "Birth date");
    case PlayersModel::Columns::Federation:
        return i18nc("@title:column Player Federation", "Federation");
    case PlayersModel::Columns::Origin:
        return i18nc("@title:column Player Origin", "Origin");
    case PlayersModel::Columns::Gender:
        return i18nc("@title:column", "Sex");
    }
    return {};
}

void PlayersModel::setColumns(const QList<int> &columns)
{
    m_columns = columns;
}

void PlayersModel::setTournament(Tournament *tournament)
{
    m_tournament = tournament;
}

void PlayersModel::setPlayers(const QList<Player *> &players)
{
    beginResetModel();
    m_players = players;
    endResetModel();
}

void PlayersModel::addPlayer(Player *player)
{
    beginInsertRows({}, static_cast<int>(m_players.size()), static_cast<int>(m_players.size()));
    m_players << player;
    endInsertRows();
}

void PlayersModel::updatePlayer(Player *player)
{
    Q_UNUSED(player);

    Q_EMIT dataChanged(index(player->startingRank() - 1, 0), index(player->startingRank() - 1, columnCount() - 1), {});
}

void PlayersModel::reloadPlayers()
{
    Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), {});
}
