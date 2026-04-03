// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCoroTask>
#include <QNetworkAccessManager>
#include <QNetworkRequestFactory>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QObject>
#include <QRestAccessManager>
#include <qqmlintegration.h>

class Account : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)

public:
    explicit Account();

    [[nodiscard]] QString accountId() const;
    [[nodiscard]] QString serverUrl() const;
    [[nodiscard]] QString username() const;

    QCoro::Task<> load();
    void login();
    QCoro::Task<> logout();

    QNetworkRequestFactory *api();
    QRestAccessManager *rest();

public Q_SLOTS:
    void setAccountId(const QString &accountId);

    void setServerUrl(const QString &serverUrl);

    void setUsername(const QString &username);

    void setAccessToken(const QString &token);

    void setRefreshToken(const QString &token);

Q_SIGNALS:
    void openUrl(const QUrl &url);

    void ready();

    void usernameChanged();

private:
    QString m_accountId;
    QString m_serverUrl;
    QString m_username;
    QString m_accessToken;
    QString m_refreshToken;

    QNetworkAccessManager m_nam;
    QOAuth2AuthorizationCodeFlow m_oauth;
    std::unique_ptr<QOAuthHttpServerReplyHandler> m_replyHandler;

    std::unique_ptr<QNetworkRequestFactory> m_api;
    std::unique_ptr<QRestAccessManager> m_rest;

    [[nodiscard]] QString accessTokenKey() const;
    [[nodiscard]] QString refreshTokenKey() const;

    [[nodiscard]] QString settingsGroup() const;
    QCoro::Task<> saveAccount(QString accessToken, QString refreshToken);
    QCoro::Task<> fetchUserDetails();
};
