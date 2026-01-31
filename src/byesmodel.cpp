// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "byesmodel.h"

#include "tournament/tournament.h"

ByesModel::ByesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tournament *ByesModel::tournament()
{
    return m_tournament;
}

void ByesModel::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }
    m_tournament = tournament;
    Q_EMIT tournamentChanged();
}

Player *ByesModel::player()
{
    return m_player;
}

void ByesModel::setPlayer(Player *player)
{
    if (m_player == player) {
        return;
    }
    m_player = player;
    Q_EMIT playerChanged();
}

int ByesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_tournament->numberOfRounds();
}

QVariant ByesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const int round = index.row() + 1;
    const auto pairings = m_tournament->pairingsOfPlayer(m_player);
    const auto pairing = pairings.at(round - 1);

    switch (role) {
    case Qt::DisplayRole: {
        if (pairing == nullptr) {
            return QString();
        }
        const auto opponent = pairing->opponent(m_player);
        if (opponent == nullptr) {
            return QString();
        }
        const auto color = Pairing::humanColorString(pairing->colorOfPlayer(m_player));
        return i18nc("Pieces Color · Player Name", "%1 · %2", color, opponent->name());
    }
    case ByesModel::Roles::ResultRole: {
        if (pairing == nullptr) {
            return QVariant::fromValue(Pairing::PartialResult::Unknown);
        }
        return QVariant::fromValue(pairing->getResultOfPlayer(m_player));
    }
    case ByesModel::Roles::ResultStringRole: {
        if (pairing == nullptr) {
            return Pairing::partialResultToString(Pairing::PartialResult::Unknown);
        }
        return Pairing::partialResultToString(pairing->getResultOfPlayer(m_player));
    }
    }

    return {};
}

QHash<int, QByteArray> ByesModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "name"},
        {ByesModel::Roles::ResultRole, "result"},
        {ByesModel::Roles::ResultStringRole, "resultString"},
    };
}

void ByesModel::reload()
{
    Q_EMIT dataChanged(index(0), index(rowCount() - 1));
}

void ByesModel::retire()
{
    if (const auto ok = m_tournament->retire(m_player); !ok) {
        Q_EMIT errorOcurred(ok.error());
    }

    Q_EMIT dataChanged(index(0), index(rowCount() - 1));
}

#include "moc_byesmodel.cpp"
