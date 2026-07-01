// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <qqmlregistration.h>

class Tournament;

class ArbitersModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Tournament *tournament READ tournament WRITE setTournament NOTIFY tournamentChanged)

public:
    enum Roles {
        Role = Qt::UserRole,
        Name,
        Title,
        Id,
    };
    Q_ENUM(Roles);

    explicit ArbitersModel(QObject *parent = nullptr);

    Tournament *tournament();

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addArbiter();
    Q_INVOKABLE void deleteArbiter(int row);

public Q_SLOTS:
    void setTournament(Tournament *tournament);

Q_SIGNALS:
    void tournamentChanged();

private:
    Tournament *m_tournament = nullptr;
};
