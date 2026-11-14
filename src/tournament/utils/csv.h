// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include <QStringList>
#include <QTextStream>

class CsvFile
{
public:
    CsvFile(QTextStream *device);

    [[nodiscard]] QStringList headers() const;

    QStringList readHeader();

    QStringList readLine();

private:
    QTextStream *m_device;

    QChar m_separator{u'\t'};
    QStringList m_headers;
};
