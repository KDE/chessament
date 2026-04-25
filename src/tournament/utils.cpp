// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"

#include <QCoreApplication>
#include <QRegularExpression>

using namespace Qt::StringLiterals;

namespace Utils
{

QString normalize(const QString &text)
{
    return text.normalized(QString::NormalizationForm_KD).remove(QRegularExpression{u"[^a-zA-Z0-9\\s\\.,-_\\(\\)]"_s});
}

QUrl maybeAddExtension(const QUrl &fileUrl, const QString &extension)
{
    auto result{fileUrl};
    if (!KSandbox::isInside() && !fileUrl.path().endsWith(extension)) {
        result.setPath(fileUrl.path() + extension);
    }
    return result;
}

QString userAgent()
{
    static QString userAgent = u"Chessament/"_s % QCoreApplication::applicationVersion() % u" (+https://apps.kde.org/chessament/;)"_s;
    return userAgent;
}
}
