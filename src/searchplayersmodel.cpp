// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchplayersmodel.h"

#include <KLocalizedString>
#include <QCoroFuture>
#include <QtConcurrentRun>

#include "ratinglists/ratinglistsmanager.h"

SearchPlayersModel::SearchPlayersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SearchPlayersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_players.size());
}

QVariant SearchPlayersModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto player = m_players[index.row()];

    switch (role) {
    case SearchPlayersModel::Role::NameRole:
        return player.name();
    case SearchPlayersModel::Role::Description: {
        if (player.standardRating() == 0 && player.origin().isEmpty()) {
            return QString{};
        }
        if (player.standardRating() == 0) {
            return player.origin();
        }
        if (player.origin().isEmpty()) {
            return i18nc("%1 is the player's rating", "Rating: %1", player.standardRating());
        }
        return i18nc("%1 is the player's rating, %2 is the player's origin", "Rating: %1 · %2", player.standardRating(), player.origin());
    }
    case SearchPlayersModel::Role::RatingRole:
        return player.standardRating();
    case SearchPlayersModel::Role::PlayerRole:
        return QVariant::fromValue(player);
    }

    return {};
}

QHash<int, QByteArray> SearchPlayersModel::roleNames() const
{
    return {
        {Role::NameRole, "name"},
        {Role::Description, "description"},
        {Role::RatingRole, "rating"},
        {Role::PlayerRole, "player"},
    };
}

QCoro::QmlTask SearchPlayersModel::search(const QString &text)
{
    return searchPlayers(text);
}

QCoro::Task<int> SearchPlayersModel::searchPlayers(const QString &text)
{
    if (text.isEmpty()) {
        beginResetModel();
        m_players.clear();
        endResetModel();
        co_return 0;
    }

    const auto players = co_await QtConcurrent::run([text]() {
        return RatingListsManager::searchPlayers(text);
    });

    if (!players) {
        qWarning() << players.error();
        co_return 0;
    }

    beginResetModel();
    m_players = (*players).mid(0, 10);
    endResetModel();

    co_return m_players.size();
}

#include "moc_searchplayersmodel.cpp"
