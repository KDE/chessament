// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <KLocalizedString>
#include <QAbstractTableModel>

#include "tournament/tournament.h"

class Standing;

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

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setStandings(QList<Standing> standings);
    void setTournament(Tournament *tournament);

private:
    Tournament *m_tournament;
    QList<Standing> m_standings;
};
