// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCoroTask>
#include <QObject>
#include <QWebSocket>

class Account;
class Tournament;

class SyncEngine : public QObject
{
public:
    explicit SyncEngine(Account *account, Tournament *tournament);

    QCoro::Task<> upload();

    void start();

public Q_SLOTS:
    void onConnected();

    void onDisconnected();

    void onError(QAbstractSocket::SocketError error);

    void onTextMessageReceived(const QString &message);

private:
    void uploadTournament();

    Account *m_account;
    Tournament *m_tournament;

    QWebSocket m_webSocket;
};
