// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syncengine.h"

#include <QDebug>

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
