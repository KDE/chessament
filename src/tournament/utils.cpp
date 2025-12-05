// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"

#include <QRegularExpression>

using namespace Qt::StringLiterals;

namespace Utils
{

QString normalize(const QString &text)
{
    return text.normalized(QString::NormalizationForm_KD).remove(QRegularExpression{u"[^a-zA-Z0-9\\s\\.,-_\\(\\)]"_s});
}

}
