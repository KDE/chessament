// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tournament.h"

constexpr static QStringView trfDateFormat = u"yy/MM/dd";

class TRFReader
{
public:
    struct pairing {
        int round;
        int white;
        int black;
    };

private:
    explicit TRFReader(Tournament *tournament);

    std::expected<void, QString> read(QTextStream *trf);

    std::expected<void, QString> readField(QStringView line);
    std::expected<void, QString> readDates(QStringView line);
    std::expected<void, QString> readPlayer(QStringView line);
    std::expected<void, QString> readPairing(int startingRank, int round, QStringView text);

    Tournament *m_tournament;

    QHash<int, Player *> m_players;
    QHash<pairing, Pairing::Result> m_pairings;

    friend class Tournament;
};

bool operator==(const TRFReader::pairing &a, const TRFReader::pairing &b) noexcept;

namespace std
{
template<>
struct hash<TRFReader::pairing> {
    size_t operator()(const TRFReader::pairing &key, size_t seed = 0) const
    {
        return qHashMulti(seed, key.round, key.white, key.black);
    }
};
}
