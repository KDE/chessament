// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <KLocalizedString>
#include <QAbstractTableModel>

#include "libtournament/tournament.h"

class StandingsModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum StandingRoles {
        RankRole = 0,
        StartingRankRole,
        TitleRole,
        NameRole,
    };
    Q_ENUM(StandingRoles)

    explicit StandingsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setTournament(Tournament *tournament);

private:
    Tournament *m_tournament;
    QList<PlayerTiebreaks> m_standings;
};
