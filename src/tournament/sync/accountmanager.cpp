// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "accountmanager.h"

#include <KSharedConfig>
#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QQmlEngine>
#include <qt6keychain/keychain.h>

#include <KLocalizedString>

#include "accountconfig.h"

using namespace Qt::Literals::StringLiterals;

AccountManager::AccountManager()
{
    loadAccounts();
}

void AccountManager::loadAccounts()
{
    auto config = KSharedConfig::openStateConfig();
    for (const auto &id : config->groupList()) {
        if (!id.startsWith("Account-"_L1)) {
            continue;
        }

        const auto config = new AccountConfig{id};

        auto account = std::make_unique<Account>();
        account->setAccountId(config->accountId());
        account->setServerUrl(config->serverUrl());
        account->setUsername(config->username());

        account->load();

        addAccount(std::move(account));
    }
}

int AccountManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_accounts.size();
}

QVariant AccountManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto account = m_accounts.at(index.row()).get();

    switch (role) {
    case Qt::DisplayRole:
        return account->username();
    case AccountManager::Roles::AccountRole: {
        QQmlEngine::setObjectOwnership(account, QJSEngine::CppOwnership);
        return QVariant::fromValue(account);
    }
    case AccountManager::Roles::DomainRole:
        return account->serverUrl();
    }

    return {};
}

QHash<int, QByteArray> AccountManager::roleNames() const
{
    return {
        {Qt::DisplayRole, "username"},
        {AccountManager::Roles::AccountRole, "account"},
        {AccountManager::Roles::DomainRole, "domain"},
    };
}

Account *AccountManager::first()
{
    if (m_accounts.empty()) {
        qWarning() << "no account found";
        return nullptr;
    }

    auto account = m_accounts.front().get();
    QQmlEngine::setObjectOwnership(account, QJSEngine::CppOwnership);
    return account;
}

QList<Account *> AccountManager::accounts()
{
    QList<Account *> result{};
    for (auto &a : m_accounts) {
        result << a.get();
    }
    return result;
}

void AccountManager::addAccount(std::unique_ptr<Account> account)
{
    beginInsertRows({}, m_accounts.size(), m_accounts.size());
    m_accounts.push_back(std::move(account));
    endInsertRows();
}

void AccountManager::createAccount(const QString &serverUrl)
{
    auto account = new Account();
    account->setServerUrl(serverUrl);

    connect(account, &Account::openUrl, this, [this](const QUrl &url) {
        Q_EMIT openUrl(url);
    });
    connect(account, &Account::ready, this, [this, account]() {
        addAccount(std::unique_ptr<Account>(account));
    });

    account->login();
}

QCoro::QmlTask AccountManager::logout(int row)
{
    return doLogout(row);
}

QCoro::Task<> AccountManager::doLogout(int row)
{
    auto account = m_accounts.at(row).get();
    co_await account->logout();

    beginRemoveRows({}, row, row);
    m_accounts.erase(m_accounts.begin() + row);
    endRemoveRows();
}

QCoro::QmlTask AccountManager::isChessamentServer(const QString &serverUrl)
{
    return checkServer(serverUrl);
}

QCoro::Task<bool> AccountManager::checkServer(const QString &serverUrl)
{
    auto url = QUrl::fromUserInput(serverUrl);
    if (url.scheme() != "http"_L1 && url.scheme() != "https"_L1) {
        co_return false;
    }
    if (!url.isValid()) {
        co_return false;
    }
    url.setPath(u"/api/v1/instance"_s);

    QNetworkAccessManager manager;

    QNetworkRequest request{url};
    const QString userAgent = "Chessament/"_L1 + QCoreApplication::applicationVersion() + " (+https://apps.kde.org/chessament/)"_L1;
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    auto *reply = co_await manager.get(request);

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error();
        co_return false;
    }

    co_return true;
}
