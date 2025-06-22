// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "playersmodel.h"

#include <KLocalizedString>

PlayersModel::PlayersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PlayersModel::rowCount(const QModelIndex &) const
{
    return m_players->size();
}

int PlayersModel::columnCount(const QModelIndex &) const
{
    return 10;
}

QVariant PlayersModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    auto player = m_players->at(index.row()).get();

    switch (index.column()) {
    case StartingRankRole:
        return player->startingRank();
    case TitleRole:
        return Player::titleString(player->title());
    case NameRole:
        return player->name();
    case SurnameRole:
        return player->surname();
    case RatingRole:
        return player->rating();
    case NationalRatingRole:
        return player->nationalRating();
    case PlayerIdRole:
        return player->playerId();
    case BirthDateRole:
        return player->birthDate();
    case FederationRole:
        return player->federation();
    case OriginRole:
        return player->origin();
    case SexRole:
        return player->sex();
    }
    return QVariant();
}

bool PlayersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if (!index.isValid()) {
        return false;
    }

    auto player = m_players->at(index.row()).get();

    switch (index.column()) {
    case StartingRankRole:
        player->setStartingRank(value.toInt());
        break;
    case TitleRole:
        player->setTitle(Player::titleForString(value.toString()));
        break;
    case NameRole:
        player->setName(value.toString());
        break;
    case SurnameRole:
        player->setSurname(value.toString());
        break;
    case RatingRole:
        player->setRating(value.toInt());
        break;
    case NationalRatingRole:
        player->setNationalRating(value.toInt());
        break;
    case PlayerIdRole:
        player->setPlayerId(value.toString());
        break;
    case BirthDateRole:
        player->setBirthDate(value.toString());
        break;
    case FederationRole:
        player->setFederation(value.toString());
        break;
    case OriginRole:
        player->setOrigin(value.toString());
        break;
    case SexRole:
        player->setSex(value.toString());
        break;
    }

    Q_EMIT dataChanged(index, index, {});
    Q_EMIT playerChanged(player);

    return true;
}

QHash<int, QByteArray> PlayersModel::roleNames() const
{
    return {{Qt::DisplayRole, "displayName"}};
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
    switch (section) {
    case StartingRankRole:
        return i18nc("@title:column", "No");
    case TitleRole:
        return i18nc("@title:column", "Title");
    case NameRole:
        return i18nc("@title:column", "Name");
    case SurnameRole:
        return i18nc("@title:column", "Surname");
    case RatingRole:
        return i18nc("@title:column", "Rating");
    case NationalRatingRole:
        return i18nc("@title:column", "National rating");
    case PlayerIdRole:
        return i18nc("@title:column", "Player ID");
    case BirthDateRole:
        return i18nc("@title:column", "Birth date");
    case FederationRole:
        return i18nc("@title:column", "Federation");
    case OriginRole:
        return i18nc("@title:column", "Origin");
    case SexRole:
        return i18nc("@title:column", "Sex");
    }
    return QVariant();
}

void PlayersModel::setPlayers(std::vector<std::unique_ptr<Player>> *players)
{
    beginResetModel();
    m_players = players;
    endResetModel();
}

void PlayersModel::addPlayer(Player *player)
{
    Q_UNUSED(player);

    beginInsertRows({}, m_players->size() - 1, m_players->size() - 1);
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
