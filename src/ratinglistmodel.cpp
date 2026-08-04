// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglistmodel.h"

#include <KLocalizedString>
#include <QCoroFuture>
#include <QtConcurrentRun>

RatingListModel::RatingListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_lists = RatingList::lists();
}

int RatingListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(m_lists.size());
}

QVariant RatingListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

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
        {RatingListModel::RatingListRole::ListIDRole, "listId"},
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

QCoro::Task<QString> RatingListModel::importRatingListImpl(const QString &name, const QString &url)
{
    auto listUrl = QUrl::fromUserInput(url);

    auto list = std::make_unique<RatingList>(name);

    connect(list.get(), &RatingList::statusChanged, this, [this](const QString &status) {
        setStatus(status);
    });

    const auto result = co_await list->import(listUrl);

    if (!result) {
        co_return result.error();
    }

    beginInsertRows({}, rowCount(), rowCount());
    m_lists.push_back(std::move(list));
    endInsertRows();

    co_return {};
}

QCoro::QmlTask RatingListModel::removeList(int row)
{
    return remove(row);
}

QCoro::Task<> RatingListModel::remove(int row)
{
    const auto list = m_lists.at(row).get();
    const auto id = list->id();

    co_await QtConcurrent::run([id]() {
        RatingList::remove(id);
    });

    beginRemoveRows({}, row, row);
    m_lists.erase(m_lists.begin() + row);
    endRemoveRows();
}

bool RatingListModel::isSupportedUrl(const QString &location)
{
    return RatingList::isSupportedUrl(location);
}

#include "moc_ratinglistmodel.cpp"
