// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QTextStream>

#include <expected>

#include "ratinglist.h"

class RatingListReader
{
public:
    RatingListReader(RatingList *list);

    virtual ~RatingListReader() = default;

    RatingList *list();

    virtual std::expected<uint, QString> readPlayers(QTextStream *stream) = 0;

private:
    RatingList *m_list;
};
