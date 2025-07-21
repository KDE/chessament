// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pairingmodel.h"

#include <KLocalizedString>

PairingModel::PairingModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PairingModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_pairings == nullptr) {
        return 0;
    }
    return static_cast<int>(m_pairings->size());
}

int PairingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 6;
}

QVariant PairingModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    if (!index.isValid()) {
        return {};
    }

    Q_ASSERT(index.row() >= 0 && static_cast<std::size_t>(index.row()) < m_pairings->size());

    auto pairing = m_pairings->at(index.row()).get();
    int column = m_columns.at(index.column());

    if (role == Qt::DisplayRole) {
        switch (column) {
        case Board:
            if (Pairing::isBye(pairing->whiteResult())) {
                return QString{};
            }
            return pairing->board();
        case WhiteStartingRank:
            return pairing->whitePlayer()->startingRank();
        case WhiteName:
            return pairing->whitePlayer()->fullName();
        case Result:
            if (pairing->whiteResult() == Pairing::PartialResult::Unknown) {
                return QString{};
            }
            return pairing->resultString();
        case BlackName:
            if (pairing->blackPlayer() == nullptr) {
                return QLatin1String("");
            }
            return pairing->blackPlayer()->fullName();
        case BlackStartingRank:
            if (pairing->blackPlayer() == nullptr) {
                return QLatin1String("");
            }
            return pairing->blackPlayer()->startingRank();
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (column) {
        case Result:
            return Qt::AlignCenter;
        case WhiteName:
        case BlackName:
            return Qt::AlignLeading;
        default:
            return Qt::AlignTrailing;
        }
    }

    return {};
}

QHash<int, QByteArray> PairingModel::roleNames() const
{
    return {{Qt::DisplayRole, "displayName"}};
}

QVariant PairingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    switch (m_columns.at(section)) {
    case Board:
        return i18nc("@title:column", "Board");
    case WhiteStartingRank:
        return i18nc("@title:column", "No");
    case WhiteName:
        return i18nc("@title:column", "White player");
    case Result:
        return i18nc("@title:column", "Result");
    case BlackName:
        return i18nc("@title:column", "Black Player");
    case BlackStartingRank:
        return i18nc("@title:column", "No");
    }

    return {};
}

void PairingModel::setColumns(const QList<int> &columns)
{
    m_columns = columns;
}

void PairingModel::setPairings(std::vector<std::unique_ptr<Pairing>> *pairings)
{
    m_pairings = pairings;

    if (m_pairings == nullptr) {
        return;
    }

    auto rowDiff = static_cast<long int>(pairings->size() - m_currentRows);

    if (rowDiff > 0) {
        beginInsertRows({}, static_cast<int>(m_currentRows), static_cast<int>(pairings->size() - 1));
    } else if (rowDiff < 0) {
        beginRemoveRows({}, static_cast<int>(pairings->size()), static_cast<int>(m_currentRows - 1));
    }

    m_currentRows = m_pairings->size();

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
    Q_ASSERT(static_cast<std::size_t>(board) < m_pairings->size());
    auto pairing = m_pairings->at(board).get();
    QQmlEngine::setObjectOwnership(pairing, QJSEngine::CppOwnership);
    return pairing;
}
