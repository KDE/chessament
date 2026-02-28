// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglistmodel.h"

#include <QCoroFuture>
#include <QtConcurrent>

RatingListModel::RatingListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_lists = RatingList::lists();
}

int RatingListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_lists.size();
}

QVariant RatingListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto list = m_lists.at(index.row()).get();

    switch (role) {
    case Qt::DisplayRole:
        return list->name();
    case RatingListModel::RatingListRole::ListIDRole:
        return list->id();
    }

    return {};
}

QHash<int, QByteArray> RatingListModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "name"},
        {RatingListModel::RatingListRole::ListIDRole, "listid"},
    };
}

QString RatingListModel::status() const
{
    return m_status;
}

void RatingListModel::setStatus(const QString &status)
{
    m_status = status;
    Q_EMIT statusChanged();
}

QCoro::QmlTask RatingListModel::importRatingList(const QString &name, const QString &url)
{
    return importRatingListImpl(name, url);
}

QCoro::Task<> RatingListModel::importRatingListImpl(const QString &name, const QString &url)
{
    const auto list = std::make_unique<RatingList>();

    connect(list.get(), &RatingList::statusChanged, this, [this](const QString &status) {
        setStatus(status);
    });

    const auto result = co_await list->import(name, QUrl{url});

    if (!result) {
        setStatus(result.error());
        co_return;
    }

    beginResetModel();
    m_lists = RatingList::lists();
    endResetModel();
}

QCoro::QmlTask RatingListModel::removeList(int row)
{
    return remove(row);
}

QCoro::Task<> RatingListModel::remove(int row)
{
    const auto list = m_lists.at(row).get();
    const auto id = list->id();

    beginRemoveRows({}, row, row);
    m_lists.erase(m_lists.begin() + row);
    endRemoveRows();

    co_await QtConcurrent::run([id]() {
        RatingList::remove(id);
    });
}

#include "moc_ratinglistmodel.cpp"
