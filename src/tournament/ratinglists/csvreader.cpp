// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "csvreader.h"

#include "utils/csv.h"

CsvRatingListReader::CsvRatingListReader(RatingList *list)
    : RatingListReader(list)
{
}

std::expected<void, QString> CsvRatingListReader::readPlayers(QTextStream *stream)
{
    stream->setEncoding(QStringConverter::Encoding::Latin1);

    CsvFile csv{stream};

    const auto header = csv.readHeader();

    while (!stream->atEnd()) {
        const auto line = csv.readLine();

        if (line.size() != 8) {
            continue;
        }

        const auto &playerId = line[0];
        const auto &name = line[1];
        const auto &gender = line[2];
        const auto &k = line[3];

        bool ok;
        const auto rating = line[4].toInt(&ok);
        if (!ok || rating < 0) {
            continue;
        }

        const auto &title = line[5];
        const auto &category = line[6];
        const auto &club = line[7];

        const auto player = RatingListPlayer{QString{},
                                             name,
                                             QString{},
                                             gender,
                                             title,
                                             QString{},
                                             rating,
                                             0,
                                             0,
                                             playerId,
                                             0,
                                             {
                                                 {u"origin"_s, club},
                                                 {u"category"_s, category},
                                                 {u"sk"_s, k},
                                             }};

        if (const auto ok = addPlayer(player); !ok) {
            return std::unexpected(ok.error());
        }
    }

    return {};
}
