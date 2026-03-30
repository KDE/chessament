// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qqml.h"
#include <QAbstractListModel>
#include <QCoroQmlTask>
#include <QCoroTask>

#include "account.h"

class AccountManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum Roles {
        AccountRole = Qt::UserRole,
        DomainRole,
    };
    Q_ENUM(Roles);

    explicit AccountManager();

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE Account *first();
    QList<Account *> accounts();

    void addAccount(std::unique_ptr<Account> account);

    Q_INVOKABLE void createAccount(const QString &serverUrl);
    Q_INVOKABLE QCoro::QmlTask logout(int row);

    static Q_INVOKABLE QCoro::QmlTask isChessamentServer(const QString &serverUrl);

Q_SIGNALS:
    void openUrl(const QUrl &url);

private:
    void loadAccounts();

    static QCoro::Task<bool> checkServer(const QString &serverUrl);
    QCoro::Task<> doLogout(int row);

    std::vector<std::unique_ptr<Account>> m_accounts;
};
