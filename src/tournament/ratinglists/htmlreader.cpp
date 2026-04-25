// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "htmlreader.h"
#include "ratinglists/ratinglist.h"

HtmlRatingListReader::HtmlRatingListReader(RatingList *list)
    : RatingListReader(list)
{
}

std::expected<uint, QString> HtmlRatingListReader::readPlayers(QTextStream *stream)
{
    uint count{0};
    QList<RatingListPlayer> players{};

    stream->setEncoding(QStringConverter::Latin1);

    auto data = stream->readAll();
    data.replace("<td></tr>"_L1, "</tr>"_L1);
    if (data.endsWith("<tr><td>"_L1)) {
        data.chop(8);
        data.append(u"</table>"_s);
    }

    m_xml.addData(data);

    if (m_xml.readNextStartElement()) {
        if (m_xml.name() == "table"_L1) {
            Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "table"_L1);

            while (m_xml.readNextStartElement()) {
                if (m_xml.name() == "tr"_L1) {
                    if (count == 0) {
                        // Skip header row
                        ++count;
                        m_xml.skipCurrentElement();
                        continue;
                    }

                    const auto player = readPlayer();
                    if (!player) {
                        return std::unexpected(player.error());
                    }

                    if (!player->name.isEmpty()) {
                        players << *player;
                        ++count;
                    }
                } else {
                    m_xml.skipCurrentElement();
                }

                if (count % 100 == 0) {
                    if (const auto ok = list()->savePlayers(players); !ok) {
                        return std::unexpected(ok.error());
                    }

                    players.clear();
                }
            }
        }
    }

    if (m_xml.hasError()) {
        qWarning() << m_xml.errorString() << m_xml.lineNumber() << m_xml.columnNumber();
        return std::unexpected(m_xml.errorString());
    }

    if (const auto ok = list()->savePlayers(players); !ok) {
        return std::unexpected(ok.error());
    }

    return count;
}

std::expected<RatingListPlayer, QString> HtmlRatingListReader::readPlayer()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "tr"_L1);

    RatingListPlayer player{};
    int index{0};

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "td"_L1) {
            const QString value = m_xml.readElementText();

            switch (index) {
            case 0:
                player.name = value;
                break;
            case 1:
                player.name.append(", "_L1);
                player.name.append(value);
                break;
            case 2:
                player.extra["origin"_L1] = value;
                break;
            case 3:
                player.extra["region"_L1] = value;
                break;
            case 4:
                player.birthDate = value;
                break;
            case 5:
                player.nationalRating = value.toInt();
                break;
            case 7:
                player.standardRating = value.toInt();
                break;
            case 8:
                player.nationalId = value;
                break;
            case 10:
                player.id = value;
                break;
            case 11:
                player.gender = value;
                break;
            }

            ++index;
        } else {
            m_xml.skipCurrentElement();
        }
    }

    return player;
}
