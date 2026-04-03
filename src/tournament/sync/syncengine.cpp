// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syncengine.h"

#include <iostream>

#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QRestReply>
#include <QtConcurrent>

#include "account.h"
#include "tournament.h"

using namespace std::chrono_literals;

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

    m_timer.setSingleShot(true);
    m_timer.setInterval(2s);

    connect(&m_timer, &QTimer::timeout, this, &SyncEngine::uploadTournament);

    connect(m_tournament, &Tournament::optionChanged, this, &SyncEngine::markDirty);
    connect(m_tournament, &Tournament::playerChanged, this, &SyncEngine::markDirty);
    connect(m_tournament, &Tournament::pairingChanged, this, &SyncEngine::markDirty);
}

SyncEngine::Status SyncEngine::status()
{
    return m_status;
}

void SyncEngine::setStatus(SyncEngine::Status status)
{
    if (m_status == status) {
        return;
    }
    qDebug() << "Sync status changed to" << status;
    m_status = status;
    Q_EMIT statusChanged();
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
    setStatus(Status::Connecting);

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
    setStatus(Status::Online);

    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &SyncEngine::onTextMessageReceived);

    uploadTournament();
}

void SyncEngine::onDisconnected()
{
    setStatus(Status::Offline);
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

void SyncEngine::markDirty()
{
    m_timer.start();
}

void SyncEngine::uploadTournament()
{
    qDebug() << "Uploading tournament";

    auto tournament = m_tournament->toJson();
    const auto rounds = tournament.take("rounds"_L1);
    const auto json = QJsonObject{
        {"type"_L1, "tournament.upload"_L1},
        {"tournament"_L1, tournament},
        {"rounds"_L1, rounds},
    };

    m_webSocket.sendTextMessage(toJson(json));
}
