// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "csv.h"

CsvFile::CsvFile(QTextStream *device)
    : m_device(device)
{
}

QStringList CsvFile::headers() const
{
    return m_headers;
}

QStringList CsvFile::readHeader()
{
    const QString line = m_device->readLine();

    m_headers = line.split(m_separator);
    return m_headers;
}

QStringList CsvFile::readLine()
{
    const QString line = m_device->readLine();
    return line.split(m_separator);
}
