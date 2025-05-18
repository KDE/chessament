// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"

#include <QRegularExpression>

using namespace Qt::StringLiterals;

namespace Utils
{

QString normalize(QString text)
{
    return text.normalized(QString::NormalizationForm_KD).remove(QRegularExpression{u"[^a-zA-Z0-9\\s]"_s});
}

}
