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

    return static_cast<int>(m_pairings.size());
}

int PairingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_columns.size());
}

QVariant PairingModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    if (!index.isValid()) {
        return {};
    }

    Q_ASSERT(index.row() >= 0 && index.row() < m_pairings.size());

    auto pairing = m_pairings.at(index.row());
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
            return pairing->whitePlayer()->name();
        case Result:
            if (pairing->whiteResult() == Pairing::PartialResult::Unknown) {
                return QString{};
            }
            return pairing->resultString();
        case BlackName:
            if (pairing->blackPlayer() == nullptr) {
                return QLatin1String("");
            }
            return pairing->blackPlayer()->name();
        case BlackStartingRank:
            if (pairing->blackPlayer() == nullptr) {
                return QLatin1String("");
            }
            return pairing->blackPlayer()->startingRank();
        }
    } else if (role == PairingModel::PairingRole::HasFinishedRole) {
        return pairing->hasFinished();
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
    return {{Qt::DisplayRole, "display"}, {PairingModel::PairingRole::HasFinishedRole, "hasFinished"}, {Qt::TextAlignmentRole, "textAlignment"}};
}

QVariant PairingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    switch (m_columns.at(section)) {
    case Board:
        return i18nc("@title:column Board Number", "Board");
    case WhiteStartingRank:
        return i18nc("@title:column White Player Starting Rank Number", "№");
    case WhiteName:
        return i18nc("@title:column Name of the White Player", "White Player");
    case Result:
        return i18nc("@title:column Game Result", "Result");
    case BlackName:
        return i18nc("@title:column Name of the Black Player", "Black Player");
    case BlackStartingRank:
        return i18nc("@title:column Black Player Starting Rank Number", "№");
    }

    return {};
}

void PairingModel::setColumns(const QList<int> &columns)
{
    m_columns = columns;
}

void PairingModel::setPairings(const QList<Pairing *> &pairings)
{
    auto rowDiff = static_cast<long int>(pairings.size() - m_pairings.size());

    if (rowDiff > 0) {
        beginInsertRows({}, static_cast<int>(m_pairings.size()), static_cast<int>(pairings.size() - 1));
    } else if (rowDiff < 0) {
        beginRemoveRows({}, static_cast<int>(pairings.size()), static_cast<int>(m_pairings.size() - 1));
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

#include "moc_pairingmodel.cpp"
