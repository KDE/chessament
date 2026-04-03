// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCoroTask>
#include <QObject>
#include <QTimer>
#include <QWebSocket>
#include <qqml.h>

#include <KLocalizedString>

class Account;
class Tournament;

class SyncEngine : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum class Status {
        Offline,
        Connecting,
        Online,
        Refused,
    };
    Q_ENUM(Status);

    explicit SyncEngine(Account *account, Tournament *tournament);

    Status status();

    static QString statusString(Status status)
    {
        switch (status) {
        case Status::Offline:
            return i18nc("@info:status", "Offline");
        case Status::Connecting:
            return i18nc("@info:status", "Connecting");
        case Status::Online:
            return i18nc("@info:status", "Online");
        case Status::Refused:
            return i18nc("@info:status", "Warning");
        }
        Q_UNREACHABLE();
    }

    QCoro::Task<> upload();

    void start();

public Q_SLOTS:
    void setStatus(Status status);

    void onConnected();

    void onDisconnected();

    void onError(QAbstractSocket::SocketError error);

    void onTextMessageReceived(const QString &message);

    void markDirty();

Q_SIGNALS:
    void statusChanged();

private:
    void uploadTournament();

    Account *m_account;
    Tournament *m_tournament;

    Status m_status = Status::Offline;

    QWebSocket m_webSocket;
    QTimer m_timer;
};
