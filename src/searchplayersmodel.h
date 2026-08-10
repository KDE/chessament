// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QCoroQmlTask>
#include <QCoroTask>
#include <qqmlregistration.h>

#include "ratinglists/ratinglistplayer.h"

class Tournament;

class SearchPlayersModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Role {
        NameRole = Qt::UserRole,
        Description,
        RatingRole,
        PlayerRole,
    };
    Q_ENUM(Role);

    explicit SearchPlayersModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QCoro::QmlTask search(const QString &text);

public Q_SLOTS:

Q_SIGNALS:

private:
    QCoro::Task<int> searchPlayers(const QString &text);

    QList<RatingListPlayer> m_players;
};
