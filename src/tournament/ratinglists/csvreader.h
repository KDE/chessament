// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#pragma once

#include "reader.h"

class CsvRatingListReader : public RatingListReader
{
public:
    explicit CsvRatingListReader(RatingList *list);

    std::expected<void, QString> readPlayers(QTextStream *stream) override;
};
