// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syncengine.h"

#include <iostream>

#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QRestReply>

#include "account.h"
#include "tournament.h"

namespace
{

QString toJson(const QJsonObject &object)
{
    return QString::fromUtf8(QJsonDocument{object}.toJson(QJsonDocument::JsonFormat::Compact));
}

}

SyncEngine::SyncEngine(Account *account, Tournament *tournament)
    : m_account(account)
    , m_tournament(tournament)
{
    Q_ASSERT(m_account);
    Q_ASSERT(m_tournament);
}

QCoro::Task<> SyncEngine::upload()
{
    const auto data = m_tournament->toJson();
    const auto json = QJsonDocument{data};

    auto api = m_account->api();
    const auto request = api->createRequest("tournaments/upload"_L1);
    const auto reply = m_account->rest()->post(request, json);

    co_await qCoro(reply).waitForFinished();

    auto result = QRestReply{reply};

    qDebug() << result.httpStatus();
    qDebug() << result.readJson();
}

void SyncEngine::start()
{
    connect(&m_webSocket, &QWebSocket::connected, this, &SyncEngine::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &SyncEngine::onDisconnected);
    connect(&m_webSocket, &QWebSocket::errorOccurred, this, &SyncEngine::onError);

    QUrl url{m_account->api()->baseUrl()};
    url.setScheme(u"ws"_s);
    url.setPath(u"/api/v1/sync"_s);

    QUrl origin{url};
    origin.setPath({});

    QNetworkRequest request{url};
    request.setRawHeader("Origin", origin.toString().toLatin1());
    request.setRawHeader("Authorization", "Bearer " + m_account->api()->bearerToken());

    m_webSocket.open(request);
}

void SyncEngine::onConnected()
{
    qDebug() << "connected to websocket";

    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &SyncEngine::onTextMessageReceived);

    uploadTournament();
}

void SyncEngine::onDisconnected()
{
    qDebug() << "disconnected";
}

void SyncEngine::onError(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    qDebug() << m_webSocket.errorString();
}

void SyncEngine::onTextMessageReceived(const QString &message)
{
    std::cout << message.toStdString() << '\n' << std::flush;
}

void SyncEngine::uploadTournament()
{
    auto tournament = m_tournament->toJson();
    const auto rounds = tournament.take("rounds"_L1);
    const auto json = QJsonObject{
        {"type"_L1, "tournament.upload"_L1},
        {"tournament"_L1, tournament},
        {"rounds"_L1, rounds},
    };

    m_webSocket.sendTextMessage(toJson(json));
}
