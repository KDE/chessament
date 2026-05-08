// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <qqmlregistration.h>

class Tournament;

class TimeControlModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Tournament *tournament READ tournament WRITE setTournament NOTIFY tournamentChanged)

public:
    enum Roles {
        Moves,
        Time,
        Increment,
    };
    Q_ENUM(Roles);

    explicit TimeControlModel(QObject *parent = nullptr);

    Tournament *tournament();

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addPeriod();
    Q_INVOKABLE void deletePeriod(int row);

public Q_SLOTS:
    void setTournament(Tournament *tournament);

Q_SIGNALS:
    void tournamentChanged();
    void errorOcurred(const QString &error);

private:
    Tournament *m_tournament;
};
