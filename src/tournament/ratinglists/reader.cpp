// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "reader.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

RatingListReader::RatingListReader(RatingList *list)
    : m_list(list)
{
}

RatingList *RatingListReader::list()
{
    return m_list;
}
