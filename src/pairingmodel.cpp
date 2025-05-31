// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pairingmodel.h"

#include <KLocalizedString>

PairingModel::PairingModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PairingModel::rowCount(const QModelIndex &) const
{
    return m_pairings.size();
}

int PairingModel::columnCount(const QModelIndex &) const
{
    return 6;
}

QVariant PairingModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    Q_ASSERT(index.row() >= 0 && index.row() < m_pairings.size());

    auto pairing = m_pairings.at(index.row());

    switch (index.column()) {
    case BoardRole:
        if (Pairing::isBye(pairing->whiteResult())) {
            return u""_s;
        }
        return pairing->board();
    case WhiteStartingRankRole:
        return pairing->whitePlayer()->startingRank();
    case WhiteNameRole:
        return pairing->whitePlayer()->fullName();
    case ResultRole:
        return pairing->resultString();
    case BlackNameRole:
        if (pairing->blackPlayer() == nullptr) {
            return QLatin1String("");
        }
        return pairing->blackPlayer()->fullName();
    case BlackStartingRankRole:
        if (pairing->blackPlayer() == nullptr) {
            return QLatin1String("");
        }
        return pairing->blackPlayer()->startingRank();
    }
    return QVariant();
}

QHash<int, QByteArray> PairingModel::roleNames() const
{
    return {{Qt::DisplayRole, "displayName"}};
}

QVariant PairingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    switch (section) {
    case BoardRole:
        return i18nc("@title:column", "Board");
    case WhiteStartingRankRole:
        return i18nc("@title:column", "No");
    case WhiteNameRole:
        return i18nc("@title:column", "White player");
    case ResultRole:
        return i18nc("@title:column", "Result");
    case BlackNameRole:
        return i18nc("@title:column", "Black Player");
    case BlackStartingRankRole:
        return i18nc("@title:column", "No");
    }
    return QVariant();
}

void PairingModel::setPairings(QList<Pairing *> pairings)
{
    int rowDiff = pairings.size() - m_pairings.size();

    if (rowDiff > 0) {
        beginInsertRows({}, m_pairings.size(), pairings.size() - 1);
    } else if (rowDiff < 0) {
        beginRemoveRows({}, pairings.size(), m_pairings.size() - 1);
    }

    m_pairings = pairings;

    if (rowDiff > 0) {
        endInsertRows();
    } else if (rowDiff < 0) {
        endRemoveRows();
    }

    Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), {});
}

void PairingModel::updatePairing(int board)
{
    Q_EMIT dataChanged(index(board - 1, 0), index(board - 1, columnCount() - 1), {});
}

Pairing *PairingModel::getPairing(int board)
{
    if (board < 0) {
        return nullptr;
    }
    Q_ASSERT(board < m_pairings.size());
    auto pairing = m_pairings.at(board);
    QQmlEngine::setObjectOwnership(pairing, QJSEngine::CppOwnership);
    return pairing;
}
