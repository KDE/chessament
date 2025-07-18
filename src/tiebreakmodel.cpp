// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tiebreakmodel.h"
#include <qnamespace.h>

#include "libtournament/tournament.h"

TiebreakModel::TiebreakModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tournament *TiebreakModel::tournament()
{
    return m_tournament;
}

void TiebreakModel::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }
    m_tournament = tournament;
    Q_EMIT tournamentChanged();
}

int TiebreakModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_tournament->tiebreaks().size();
}

QVariant TiebreakModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
        return m_tournament->tiebreaks()[index.row()]->name();
    }

    return {};
}
