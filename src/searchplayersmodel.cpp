// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchplayersmodel.h"

SearchPlayersModel::SearchPlayersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SearchPlayersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_players.size();
}

QVariant SearchPlayersModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto player = m_players[index.row()];

    switch (role) {
    case SearchPlayersModel::Role::NameRole:
        return player.name;
    case SearchPlayersModel::Role::RatingRole:
        return player.standardRating;
    case SearchPlayersModel::Role::PlayerRole:
        return QVariant::fromValue(player);
    }

    return {};
}

QHash<int, QByteArray> SearchPlayersModel::roleNames() const
{
    return {
        {Role::NameRole, "name"},
        {Role::RatingRole, "rating"},
        {Role::PlayerRole, "player"},
    };
}

void SearchPlayersModel::search(const QString &text)
{
    const auto players = RatingList::searchPlayers(text);

    if (!players) {
        qWarning() << players.error();
        return;
    }

    beginResetModel();
    m_players = (*players).mid(0, 10);
    endResetModel();
}

#include "moc_searchplayersmodel.cpp"
