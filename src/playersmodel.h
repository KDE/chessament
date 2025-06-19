// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractTableModel>

#include "libtournament/player.h"

class PlayersModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum PlayerRoles {
        StartingRankRole = 0,
        TitleRole,
        NameRole,
        SurnameRole,
        RatingRole,
        NationalRatingRole,
        PlayerIdRole,
        BirthDateRole,
        FederationRole,
        OriginRole,
        SexRole,
    };
    Q_ENUM(PlayerRoles)

    explicit PlayersModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setPlayers(std::vector<std::unique_ptr<Player>> *players);
    void addPlayer(Player *player);
    void updatePlayer(Player *player);

Q_SIGNALS:
    void playerChanged(Player *player);

private:
    std::vector<std::unique_ptr<Player>> *m_players;
};
