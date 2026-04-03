// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>
#include <QWebSocket>

#include "syncengine.h"

class Account;
class Tournament;

class CloudEngine : public SyncEngine
{
    Q_OBJECT

public:
    explicit CloudEngine(Account *account, Tournament *tournament);

    void start() override;

public Q_SLOTS:
    void onConnected();

    void onDisconnected();

    void onError(QAbstractSocket::SocketError error);

    void onTextMessageReceived(const QString &message);

    void markDirty();

private:
    void uploadTournament();

    Account *m_account;
    Tournament *m_tournament;

    QWebSocket m_webSocket;
    QTimer m_timer;
};
