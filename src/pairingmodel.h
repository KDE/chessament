// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "tournament/pairing.h"

#include <QAbstractTableModel>

class Tournament;

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

    enum PairingRole {
        HasFinishedRole = Qt::UserRole,
    };
    Q_ENUM(PairingRole)

    explicit PairingModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setColumns(const QList<int> &columns);
    void setTournament(Tournament *tournament);
    void setPairings(const QList<Pairing *> &pairings);
    Q_INVOKABLE bool setResult(int board, Qt::Key key);
    Q_INVOKABLE bool setResult(int board, Pairing::PartialResult whiteResult, Pairing::PartialResult blackResult);
    Q_INVOKABLE Pairing *pairing(int board);

Q_SIGNALS:
    void pairingChanged();

    void errorOcurred(const QString &error);

private:
    Tournament *m_tournament;
    QList<Pairing *> m_pairings;

    QList<int> m_columns = {Board, WhiteStartingRank, WhiteName, Result, BlackName, BlackStartingRank};
};
