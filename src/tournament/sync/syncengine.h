// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <KLocalizedString>
#include <QObject>

class SyncEngine : public QObject
{
    Q_OBJECT

public:
    enum class Status {
        Offline,
        Connecting,
        Online,
        Refused,
    };
    Q_ENUM(Status);

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

    SyncEngine::Status status();

    virtual void start() = 0;

public Q_SLOTS:
    void setStatus(Status status);

Q_SIGNALS:
    void statusChanged();

private:
    Status m_status = Status::Offline;
};
