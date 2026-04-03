// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QCoroQmlTask>
#include <QCoroTask>
#include <qqml.h>

#include "account.h"

class AccountManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum Roles {
        AccountRole = Qt::UserRole,
        DomainRole,
    };
    Q_ENUM(Roles);

    static AccountManager *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        Q_UNUSED(qmlEngine);
        Q_UNUSED(jsEngine);
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    static AccountManager &instance();

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
    explicit AccountManager();

    void loadAccounts();

    static QCoro::Task<bool> checkServer(const QString &serverUrl);
    QCoro::Task<> doLogout(int row);

    std::vector<std::unique_ptr<Account>> m_accounts;
};
