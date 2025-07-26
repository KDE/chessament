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
    enum Columns {
        StartingRank,
        Title,
        Name,
        Rating,
        NationalRating,
        PlayerId,
        BirthDate,
        Federation,
        Origin,
        Gender,
    };
    Q_ENUM(Columns)

    explicit PlayersModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setColumns(const QList<int> &columns);
    void setPlayers(const QList<Player *> &players);
    void addPlayer(Player *player);
    void updatePlayer(Player *player);
    void reloadPlayers();

Q_SIGNALS:
    void playerChanged(Player *player);

private:
    QList<Player *> m_players;

    QList<int> m_columns = {StartingRank, Title, Name, Rating, NationalRating, PlayerId, BirthDate, Federation, Origin, Gender};
};
