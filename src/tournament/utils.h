// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <KSandbox>
#include <QString>
#include <QUrl>

namespace Utils
{
QString normalize(const QString &text);

QUrl maybeAddExtension(const QUrl &fileUrl, const QString &extension);

QString userAgent();
}
