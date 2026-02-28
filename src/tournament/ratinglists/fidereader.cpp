// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "fidereader.h"

FideRatingListReader::FideRatingListReader(RatingList *list)
    : RatingListReader(list)
{
}

std::expected<uint, QString> FideRatingListReader::readPlayers(QTextStream *stream)
{
    QString line;
    uint count{0};
    QList<RatingList::Player> players{};

    stream->readLine(); // Skip header

    while (stream->readLineInto(&line)) {
        if (line.size() < 162) {
            continue;
        }

        bool ok;

        const int playerId = line.sliced(0, 15).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid player id of player" << line;
            continue;
        }

        const QString name = line.sliced(15, 61).trimmed();
        const QString federation = line.sliced(76, 3);
        const QString gender = line.sliced(80, 3).trimmed();
        const QString title = line.sliced(84, 4).trimmed();
        const auto standardRating = line.sliced(113, 4).toUInt(&ok);
        if (!ok) {
            qWarning() << "invalid standard rating of player" << line;
            continue;
        }

        const auto rapidRating = line.sliced(126, 4).toUInt(&ok);
        if (!ok) {
            qWarning() << "invalid rapid rating of player" << line;
            continue;
        }

        const auto blitzRating = line.sliced(139, 4).toUInt(&ok);
        if (!ok) {
            qWarning() << "invalid blitz rating of player" << line;
            continue;
        }

        const auto otherTitles = line.sliced(94, 15).trimmed().split(u',', Qt::SkipEmptyParts);

        const auto sk = line.sliced(123, 2).toInt(&ok);
        if (!ok) {
            continue;
        }

        const auto rk = line.sliced(136, 2).toInt(&ok);
        if (!ok) {
            continue;
        }

        const auto bk = line.sliced(149, 2).toInt(&ok);
        if (!ok) {
            continue;
        }

        QJsonObject extraJson;
        if (sk != 0) {
            extraJson["sk"_L1] = sk;
        }
        if (rk != 0) {
            extraJson["rk"_L1] = rk;
        }
        if (bk != 0) {
            extraJson["bk"_L1] = bk;
        }
        if (!otherTitles.empty()) {
            extraJson["other_titles"_L1] = QJsonValue::fromVariant(otherTitles);
        }
        const auto extra = QJsonDocument{extraJson}.toJson(QJsonDocument::Compact);

        const auto birthdate = line.sliced(152, 4).toInt(&ok);
        if (!ok) {
            continue;
        }

        players << RatingList::Player{
            .id = QString::number(playerId),
            .name = name,
            .federation = federation,
            .gender = gender,
            .title = title,
            .birthDate = QString::number(birthdate),
            .standardRating = standardRating,
            .rapidRating = rapidRating,
            .blitzRating = blitzRating,
            .nationalId = QString(),
            .nationalRating = 0,
            .extra = extraJson,
        };

        ++count;

        if (count % 100 == 0) {
            if (const auto ok = list()->savePlayers(players); !ok) {
                return std::unexpected(ok.error());
            }

            players.clear();
        }
    }

    if (const auto ok = list()->savePlayers(players); !ok) {
        return std::unexpected(ok.error());
    }

    return count;
}
