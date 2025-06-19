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
    enum PairingRoles {
        BoardRole = 0,
        WhiteStartingRankRole,
        WhiteNameRole,
        ResultRole,
        BlackNameRole,
        BlackStartingRankRole,
    };
    Q_ENUM(PairingRoles)

    explicit PairingModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setPairings(std::vector<std::unique_ptr<Pairing>> *pairings);
    Q_INVOKABLE void updatePairing(int board);
    Q_INVOKABLE Pairing *getPairing(int board);

private:
    std::vector<std::unique_ptr<Pairing>> *m_pairings = nullptr;
};
