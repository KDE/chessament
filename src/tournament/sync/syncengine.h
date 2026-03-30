// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCoroTask>

class Account;
class Tournament;

class SyncEngine
{
public:
    explicit SyncEngine(Account *account, Tournament *tournament);

    QCoro::Task<> upload();

private:
    Account *m_account;

    Tournament *m_tournament;
};
