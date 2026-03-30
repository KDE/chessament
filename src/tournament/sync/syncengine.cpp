// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syncengine.h"

#include <QCoroFuture>
#include <QCoroNetworkReply>
#include <QRestReply>
#include <QtConcurrent>

#include "account.h"
#include "tournament.h"

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
