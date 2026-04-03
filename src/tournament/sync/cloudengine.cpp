// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cloudengine.h"

#include <iostream>

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

CloudEngine::CloudEngine(Account *account, Tournament *tournament)
    : m_account(account)
    , m_tournament(tournament)
{
    Q_ASSERT(m_account);
    Q_ASSERT(m_tournament);

    m_timer.setSingleShot(true);
    m_timer.setInterval(2s);

    connect(&m_timer, &QTimer::timeout, this, &CloudEngine::uploadTournament);

    connect(&m_webSocket, &QWebSocket::connected, this, &CloudEngine::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &CloudEngine::onDisconnected);
    connect(&m_webSocket, &QWebSocket::errorOccurred, this, &CloudEngine::onError);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &CloudEngine::onTextMessageReceived);

    connect(m_tournament, &Tournament::optionChanged, this, &CloudEngine::markDirty);
    connect(m_tournament, &Tournament::playerChanged, this, &CloudEngine::markDirty);
    connect(m_tournament, &Tournament::pairingChanged, this, &CloudEngine::markDirty);
}

void CloudEngine::start()
{
    setStatus(Status::Connecting);

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

void CloudEngine::onConnected()
{
    setStatus(Status::Online);

    uploadTournament();
}

void CloudEngine::onDisconnected()
{
    setStatus(Status::Offline);
}

void CloudEngine::onError(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    qDebug() << m_webSocket.errorString();
}

void CloudEngine::onTextMessageReceived(const QString &message)
{
    std::cout << message.toStdString() << '\n' << std::flush;
}

void CloudEngine::markDirty()
{
    m_timer.start();
}

void CloudEngine::uploadTournament()
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
