// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "arbitersmodel.h"

#include "tournament/tournament.h"

ArbitersModel::ArbitersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Tournament *ArbitersModel::tournament()
{
    return m_tournament;
}

void ArbitersModel::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }

    beginResetModel();
    m_tournament = tournament;
    endResetModel();

    Q_EMIT tournamentChanged();
}

int ArbitersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_tournament->arbiters().size());
}

QVariant ArbitersModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto &arbiter = m_tournament->arbiters().at(index.row());

    switch (role) {
    case ArbitersModel::Roles::Role:
        return std::to_underlying(arbiter->role());
    case ArbitersModel::Roles::Title:
        return arbiter->title();
    case ArbitersModel::Roles::Name:
        return arbiter->name();
    case ArbitersModel::Roles::Id:
        return arbiter->arbiterId();
    }

    return {};
}

bool ArbitersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

    const auto &arbiter = m_tournament->arbiters().at(index.row());

    switch (role) {
    case ArbitersModel::Roles::Role: {
        arbiter->setRole(Arbiter::Role(value.toInt()));
        break;
    }
    case ArbitersModel::Roles::Title: {
        arbiter->setTitle(value.toString());
        break;
    }
    case ArbitersModel::Roles::Name: {
        arbiter->setName(value.toString());
        break;
    }
    case ArbitersModel::Roles::Id: {
        arbiter->setArbiterId(value.toString());
        break;
    }
    default:
        return false;
    }

    m_tournament->saveArbiters();

    Q_EMIT dataChanged(this->index(index.row()), this->index(index.row()));

    return true;
}

QHash<int, QByteArray> ArbitersModel::roleNames() const
{
    return {
        {Roles::Role, "role"},
        {Roles::Title, "title"},
        {Roles::Name, "name"},
        {Roles::Id, "arbiterId"},
    };
}

void ArbitersModel::addArbiter()
{
    beginInsertRows({}, rowCount(), rowCount());
    auto arbiter = std::make_unique<Arbiter>();
    m_tournament->arbiters().push_back(std::move(arbiter));
    endInsertRows();

    m_tournament->saveArbiters();
}

void ArbitersModel::deleteArbiter(int row)
{
    beginRemoveRows({}, row, row);

    auto &arbiters = m_tournament->arbiters();
    arbiters.erase(arbiters.begin() + row);
    m_tournament->saveArbiters();

    endRemoveRows();
}

#include "moc_arbitersmodel.cpp"
