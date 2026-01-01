// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <qqmlregistration.h>

class Tournament;
class Player;

class ByesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Tournament *tournament READ tournament WRITE setTournament NOTIFY tournamentChanged)
    Q_PROPERTY(Player *player READ player WRITE setPlayer NOTIFY playerChanged)

public:
    enum Roles {
        ResultRole = Qt::UserRole,
        ResultStringRole,
    };
    Q_ENUM(Roles);

    explicit ByesModel(QObject *parent = nullptr);

    Tournament *tournament();
    Player *player();

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void setTournament(Tournament *tournament);
    void setPlayer(Player *player);

Q_SIGNALS:
    void tournamentChanged();
    void playerChanged();

private:
    Tournament *m_tournament;
    Player *m_player;
};
