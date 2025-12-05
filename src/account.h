// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QNetworkAccessManager>
#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>
#include <QQmlEngine>

class Account : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool isLogged READ isLogged WRITE setIsLogged NOTIFY isLoggedChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)

public:
    explicit Account();

    bool isLogged();
    [[nodiscard]] QString username() const;

    void login();
    Q_INVOKABLE void logout();

public Q_SLOTS:
    void setIsLogged(bool logged);
    void setUsername(const QString &username);

    void setAccessToken(const QString &token);

Q_SIGNALS:
    void isLoggedChanged();
    void usernameChanged();

private:
    bool m_isLogged;
    QString m_username;
    QString m_accessToken;

    QNetworkAccessManager m_nam;
    QOAuth2AuthorizationCodeFlow m_oauth;

    void saveAccessToken(const QString &token);
    void loadAccount();
    void fetchUserDetails();
};
