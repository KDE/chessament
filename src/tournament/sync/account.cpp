// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account.h"

#include <KSharedConfig>
#include <QCoroNetworkReply>
#include <QCoroSignal>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QOAuthHttpServerReplyHandler>
#include <QRestReply>
#include <qt6keychain/keychain.h>

#include "accountconfig.h"

using namespace Qt::Literals::StringLiterals;

Account::Account()
    : m_api(std::make_unique<QNetworkRequestFactory>())
    , m_rest(std::make_unique<QRestAccessManager>(&m_nam))
{
}

QString Account::accountId() const
{
    return m_accountId;
}

void Account::setAccountId(const QString &accountId)
{
    if (m_accountId == accountId) {
        return;
    }
    m_accountId = accountId;
}

QString Account::serverUrl() const
{
    return m_serverUrl;
}

void Account::setServerUrl(const QString &serverUrl)
{
    if (m_serverUrl == serverUrl) {
        return;
    }
    m_serverUrl = serverUrl;
    m_api->setBaseUrl(QUrl{m_serverUrl + u"/api/v1"_s});
}

QString Account::username() const
{
    return m_username;
}

void Account::setUsername(const QString &username)
{
    if (m_username == username) {
        return;
    }
    m_username = username;
    Q_EMIT usernameChanged();
}

QCoro::Task<> Account::load()
{
    Q_ASSERT(!m_accountId.isEmpty());
    Q_ASSERT(!m_serverUrl.isEmpty());

    auto job = new QKeychain::ReadPasswordJob{u"Chessament"_s};
    job->setKey(accessTokenKey());
    job->start();

    co_await qCoro(job, &QKeychain::ReadPasswordJob::finished);

    setAccessToken(job->textData());

    delete job;
    Q_EMIT ready();
    qDebug() << "Loaded account" << m_accountId << "with username" << m_username;
}

void Account::login()
{
    Q_ASSERT(!m_serverUrl.isEmpty());

    m_oauth.setAuthorizationUrl(QUrl{m_serverUrl + "/oauth2/authorize/"_L1});
    m_oauth.setTokenUrl(QUrl{m_serverUrl + "/oauth2/token/"_L1});
    m_oauth.setClientIdentifier("scY2hIzd461zzTUePINaJldKy7GEeg7YVgLLncXm"_L1);
    m_oauth.setRequestedScopeTokens({"read", "write"});

    auto handler = new QOAuthHttpServerReplyHandler(1234, this);
    m_oauth.setReplyHandler(handler);

    connect(&m_oauth, &QAbstractOAuth::authorizeWithBrowser, this, [this](const QUrl &url) {
        Q_EMIT openUrl(url);
    });
    QObject::connect(&m_oauth, &QAbstractOAuth::granted, this, [this, handler]() {
        saveAccount(m_oauth.token());
        handler->close();
    });

    if (handler->isListening()) {
        m_oauth.grant();
    }
}

QCoro::Task<> Account::logout()
{
    auto job = new QKeychain::DeletePasswordJob{u"Chessament"_s};
    job->setAutoDelete(true);
    job->setKey(accessTokenKey());
    job->start();

    co_await qCoro(job, &QKeychain::DeletePasswordJob::finished);

    auto config = KSharedConfig::openStateConfig();
    config->deleteGroup(settingsGroup());
    config->sync();
}

QNetworkRequestFactory *Account::api()
{
    Q_ASSERT(m_api);
    return m_api.get();
}

QRestAccessManager *Account::rest()
{
    Q_ASSERT(m_rest);
    return m_rest.get();
}

QString Account::accessTokenKey()
{
    Q_ASSERT(!m_accountId.isEmpty());
    return u"access-token-%1"_s.arg(m_accountId);
}

void Account::setAccessToken(const QString &token)
{
    Q_ASSERT(!token.isEmpty());

    m_accessToken = token;
    m_api->setBearerToken(m_accessToken.toLatin1());
}

QString Account::settingsGroup() const
{
    Q_ASSERT(!m_accountId.isEmpty());
    return "Account-%1"_L1.arg(m_accountId);
}

QCoro::Task<> Account::saveAccount(const QString &token)
{
    m_accountId = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);

    setAccessToken(token);

    auto job = new QKeychain::WritePasswordJob{u"Chessament"_s};
    job->setAutoDelete(true);
    job->setKey(accessTokenKey());
    job->setTextData(token);
    job->start();

    co_await qCoro(job, &QKeychain::WritePasswordJob::finished);

    const auto config = new AccountConfig{settingsGroup()};
    config->setAccountId(m_accountId);
    config->setServerUrl(m_serverUrl);
    config->save();

    co_await fetchUserDetails();
}

QCoro::Task<> Account::fetchUserDetails()
{
    const auto request = m_api->createRequest(u"status"_s);
    const auto reply = co_await m_rest->get(request);

    auto result = QRestReply{reply};
    if (!result.isSuccess() || result.httpStatus() != 200) {
        qDebug() << "error" << reply->errorString();
        co_return;
    }

    const auto json = result.readJson();
    if (!json) {
        co_return;
    }

    const auto object = json->object();
    const auto username = object["username"_L1].toString();

    setUsername(username);

    const auto config = new AccountConfig{settingsGroup()};
    Q_ASSERT(!config->accountId().isEmpty());
    config->setUsername(m_username);

    config->save();

    Q_EMIT ready();
}

#include "moc_account.cpp"
