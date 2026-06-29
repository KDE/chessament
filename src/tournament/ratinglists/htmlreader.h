// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "reader.h"

#include <QXmlStreamReader>

class HtmlRatingListReader : public RatingListReader
{
public:
    explicit HtmlRatingListReader(RatingList *list);

    std::expected<void, QString> readPlayers(QTextStream *stream) override;

    std::expected<RatingListPlayer, QString> readPlayer();

private:
    QXmlStreamReader m_xml;
};
