// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QCoroQmlTask>
#include <qqmlregistration.h>

#include "tournament/ratinglists/ratinglist.h"

class RatingListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    enum RatingListRole {
        ListIDRole = Qt::UserRole,
    };
    Q_ENUM(RatingListRole);

    explicit RatingListModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QString status() const;

    Q_INVOKABLE QCoro::QmlTask importRatingList(const QString &name, const QString &url);
    Q_INVOKABLE QCoro::QmlTask removeList(int row);

public Q_SLOTS:
    void setStatus(const QString &status);

Q_SIGNALS:
    void statusChanged();

private:
    QCoro::Task<> importRatingListImpl(const QString &name, const QString &url);
    QCoro::Task<> remove(int row);

    QString m_status;
    std::vector<std::unique_ptr<RatingList>> m_lists;
};
