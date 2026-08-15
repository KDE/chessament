// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <KSandbox>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

class QNetworkAccessManager;

namespace Utils
{
QString normalize(const QString &text);

QUrl maybeAddExtension(const QUrl &fileUrl, const QString &extension);

QString userAgent();

void updateObject(QJsonObject *destination, const QJsonObject &origin);

std::unique_ptr<QNetworkAccessManager> networkAccessManager();

QNetworkRequest createRequest(const QUrl &url);
}
