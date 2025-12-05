// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account.h"
#include "config.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QOAuthHttpServerReplyHandler>
#include <qt6keychain/keychain.h>

using namespace Qt::Literals::StringLiterals;

Account::Account()
    : QObject()
{
    loadAccount();
}

bool Account::isLogged()
{
    return m_isLogged;
}

void Account::setIsLogged(bool logged)
{
    if (m_isLogged == logged) {
        return;
    }
    m_isLogged = logged;
    Q_EMIT isLoggedChanged();
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
    setIsLogged(!m_username.isEmpty());
    Q_EMIT usernameChanged();
}

void Account::setAccessToken(const QString &token)
{
    m_accessToken = token;

    setIsLogged(!m_accessToken.isEmpty());
}

void Account::saveAccessToken(const QString &token)
{
    auto job = new QKeychain::WritePasswordJob{u"Chessament"_s};
    job->setKey(u"access_token"_s);
    job->setTextData(token);
    job->start();

    setAccessToken(token);
}

void Account::loadAccount()
{
    auto job = new QKeychain::ReadPasswordJob{u"Chessament"_s};
    job->setKey(u"access_token"_s);

    connect(job, &QKeychain::ReadPasswordJob::finished, [this, job]() {
        setAccessToken(job->textData());
    });

    job->start();
}

void Account::fetchUserDetails()
{
    QNetworkRequest request(QUrl{"http://localhost:8000/api/status"_L1});
    const QByteArray bearer = QStringLiteral("Bearer %1").arg(m_accessToken).toLocal8Bit();
    request.setRawHeader("Authorization", bearer);

    auto reply = m_nam.get(request);

    connect(reply, &QNetworkReply::finished, [reply, this]() {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
            qDebug() << "error" << reply->errorString();
            return;
        }

        const auto data = reply->readAll();
        const auto json = QJsonDocument::fromJson(data);

        if (!json.isObject()) {
            return;
        }

        const auto obj = json.object();
        const auto username = obj["username"_L1].toString();

        setUsername(username);
        Config::setUsername(username);
    });
}

void Account::login()
{
    m_oauth.setAuthorizationUrl(QUrl("http://localhost:8000/oauth2/authorize/"_L1));
    m_oauth.setAccessTokenUrl(QUrl("http://localhost:8000/oauth2/token/"_L1));
    m_oauth.setClientIdentifier("fXqdKkXDZALnUUoabnPLqYPTWs6EKIFkl8opWZif"_L1);
    m_oauth.setScope("read write"_L1);

    auto handler = new QOAuthHttpServerReplyHandler(1234, this);
    m_oauth.setReplyHandler(handler);

    connect(&m_oauth, &QAbstractOAuth::authorizeWithBrowser, this, &QDesktopServices::openUrl);
    QObject::connect(&m_oauth, &QAbstractOAuth::granted, this, [this, &handler]() {
        qDebug() << m_oauth.token().toLatin1();

        saveAccessToken(m_oauth.token());
        handler->close();
        fetchUserDetails();
    });

    if (handler->isListening()) {
        m_oauth.grant();
    }
}

void Account::logout()
{
    auto job = new QKeychain::DeletePasswordJob{u"Chessament"_s};
    job->setKey(u"access_token"_s);
    job->start();

    setUsername({});
    Config::setUsername({});
}

#include "moc_account.cpp"
