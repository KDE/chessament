// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pairingmodel.h"

#include "tournament.h"

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
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto pairing = m_pairings.at(index.row());
    const int column = m_columns.at(index.column());

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
    } else if (role == PairingModel::Roles::HasFinishedRole) {
        return pairing->hasFinished();
    } else if (role == PairingModel::Roles::PairingRole) {
        QQmlEngine::setObjectOwnership(pairing, QJSEngine::CppOwnership);
        return QVariant::fromValue(pairing);
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
    return {
        {Qt::DisplayRole, "display"},
        {PairingModel::Roles::HasFinishedRole, "hasFinished"},
        {PairingModel::Roles::PairingRole, "pairing"},
        {Qt::TextAlignmentRole, "textAlignment"},
    };
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

void PairingModel::setTournament(Tournament *tournament)
{
    m_tournament = tournament;
}

void PairingModel::setPairings(const QList<Pairing *> &pairings)
{
    const auto rowDiff = static_cast<long int>(pairings.size() - m_pairings.size());

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

bool PairingModel::setResult(int board, Qt::Key key)
{
    Pairing::Result result;

    switch (key) {
    case Qt::Key_0:
        result = {Pairing::PartialResult::Lost, Pairing::PartialResult::Win};
        break;
    case Qt::Key_1:
        result = {Pairing::PartialResult::Win, Pairing::PartialResult::Lost};
        break;
    case Qt::Key_5:
        result = {Pairing::PartialResult::Draw, Pairing::PartialResult::Draw};
        break;
    default:
        // Enter key changes to the next pairing
        return key == Qt::Key_Enter;
    }

    return setResult(board, result.first, result.second);
}

bool PairingModel::setResult(int board, Pairing::PartialResult whiteResult, Pairing::PartialResult blackResult)
{
    const auto pairing = m_pairings[board - 1];

    const Pairing::Result result = {whiteResult, blackResult};

    if (auto ok = m_tournament->setResult(pairing, result); !ok) {
        Q_EMIT errorOcurred(ok.error());
        return false;
    }

    Q_EMIT dataChanged(index(board - 1, 0), index(board - 1, columnCount() - 1), {});
    Q_EMIT pairingChanged();

    return true;
}

QVariant PairingModel::nextPendingBoardAfter(int board)
{
    for (int i = board; i < m_pairings.size(); ++i) {
        const auto pairing = m_pairings.at(i);
        if (!pairing->hasFinished()) {
            return i + 1;
        }
    }
    for (int i = 0; i < board; ++i) {
        const auto pairing = m_pairings.at(i);
        if (!pairing->hasFinished()) {
            return i + 1;
        }
    }

    return {};
}

#include "moc_pairingmodel.cpp"
