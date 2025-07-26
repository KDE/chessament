// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractTableModel>

#include "libtournament/pairing.h"

class PairingModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum Columns {
        Board = 0,
        WhiteStartingRank,
        WhiteName,
        Result,
        BlackName,
        BlackStartingRank,
    };
    Q_ENUM(Columns)

    explicit PairingModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setColumns(const QList<int> &columns);
    void setPairings(const QList<Pairing *> &pairings);
    Q_INVOKABLE void updatePairing(int board);
    Q_INVOKABLE Pairing *getPairing(int board);

private:
    QList<Pairing *> m_pairings;

    QList<int> m_columns = {Board, WhiteStartingRank, WhiteName, Result, BlackName, BlackStartingRank};
};
