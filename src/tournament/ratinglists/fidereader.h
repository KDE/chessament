// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "reader.h"

class FideRatingListReader : public RatingListReader
{
public:
    explicit FideRatingListReader(RatingList *list);

    std::expected<void, QString> readPlayers(QTextStream *stream) override;
};
