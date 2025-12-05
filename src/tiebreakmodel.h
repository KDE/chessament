// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <qqmlregistration.h>

class Tournament;

class TiebreakModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Tournament *tournament READ tournament WRITE setTournament NOTIFY tournamentChanged)

public:
    enum TiebreakRole {
        IsConfigurableRole = Qt::UserRole,
        OptionsRole,
    };
    Q_ENUM(TiebreakRole);

    explicit TiebreakModel(QObject *parent = nullptr);

    Tournament *tournament();

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addTiebreak(const QString &tiebreak);
    Q_INVOKABLE void remove(int row);
    Q_INVOKABLE void moveUp(int row);
    Q_INVOKABLE void moveDown(int row);

public Q_SLOTS:
    void setTournament(Tournament *tournament);

Q_SIGNALS:
    void tournamentChanged();

private:
    Tournament *m_tournament;
};
