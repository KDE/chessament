// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "trf.h"
#include "tournament.h"

#include <KLocalizedString>

bool operator==(const TRFReader::pairing &a, const TRFReader::pairing &b) noexcept
{
    return a.round == b.round && a.white == b.white && a.black == b.black;
}

TRFReader::TRFReader(Tournament *tournament)
    : m_tournament(tournament)
{
}

std::expected<void, QString> TRFReader::read(QTextStream *trf)
{
    QString line;
    while (!trf->atEnd()) {
        trf->readLineInto(&line);
        QStringView view(line);

        if (view.isEmpty()) {
            continue;
        }

        if (const auto ok = readField(view); !ok) {
            return ok;
        }
    }

    for (const auto &player : std::as_const(m_players)) {
        if (auto ok = m_tournament->addPlayer(std::unique_ptr<Player>(player)); !ok) {
            return ok;
        }
    }

    std::sort(m_tournament->m_players.begin(), m_tournament->m_players.end(), [](const std::unique_ptr<Player> &a, const std::unique_ptr<Player> &b) {
        return a->startingRank() < b->startingRank();
    });

    for (auto pairing = m_pairings.cbegin(), end = m_pairings.cend(); pairing != end; ++pairing) {
        const auto key = pairing.key();
        const auto result = pairing.value();

        if (!m_players.contains(key.white)) {
            return std::unexpected(i18n("Player \"%1\" not found.", key.white));
        }
        const auto whitePlayer = m_players.value(key.white);

        Player *blackPlayer = nullptr;
        if (key.black != 0) {
            if (!m_players.contains(key.black)) {
                return std::unexpected(i18n("Player \"%1\" not found.", key.black));
            }
            blackPlayer = m_players.value(key.black);
        }

        if (result.first == Pairing::PartialResult::Unknown && result.second == Pairing::PartialResult::Unknown) {
            return std::unexpected(i18n("Unknown result on pairing \"%1\" with \"%2\".", QString::number(key.white), QString::number(key.black)));
        }
        auto par = std::make_unique<Pairing>(1, whitePlayer, blackPlayer, pairing.value().first, pairing.value().second);

        if (auto ok = m_tournament->addPairing(key.round, std::move(par)); !ok) {
            return ok;
        }
    }

    m_tournament->setNumberOfRounds(m_tournament->m_rounds.size());

    if (auto ok = m_tournament->sortPairings(); !ok) {
        return ok;
    }

    m_tournament->setCurrentRound(m_tournament->m_rounds.size());
    for (int i = 1; i <= m_tournament->m_numberOfRounds; ++i) {
        if (!m_tournament->isRoundFullyPaired(i)) {
            m_tournament->setCurrentRound(i - 1);
            break;
        }
    }

    if (m_tournament->m_currentRound > 0) {
        const auto pairing = m_tournament->getPairings(1).constFirst();
        if (pairing->blackPlayer() != nullptr) {
            Tournament::InitialColor color;
            if (pairing->whitePlayer()->startingRank() < pairing->blackPlayer()->startingRank()) {
                color = Tournament::InitialColor::White;
            } else {
                color = Tournament::InitialColor::Black;
            }
            m_tournament->setInitialColor(color);
        }
    }

    return {};
}

std::expected<void, QString> TRFReader::readField(QStringView line)
{
    const auto fieldType = line.mid(0, 3);
    const auto field = Tournament::reportFieldForString(fieldType);
    const auto value = line.mid(4);

    switch (field) {
    case Tournament::ReportField::TournamentName:
        m_tournament->setName(value.trimmed().toString());
        break;
    case Tournament::ReportField::City:
        m_tournament->setCity(value.trimmed().toString());
        break;
    case Tournament::ReportField::Federation:
        m_tournament->setFederation(value.trimmed().toString());
        break;
    case Tournament::ReportField::ChiefArbiter:
        m_tournament->setChiefArbiter(value.trimmed().toString());
        break;
    case Tournament::ReportField::DeputyChiefArbiter:
        m_tournament->setDeputyChiefArbiter(value.trimmed().toString());
        break;
    case Tournament::ReportField::TimeControl:
        m_tournament->setTimeControl(value.trimmed().toString());
        break;
    case Tournament::ReportField::Calendar: {
        if (const auto ok = readDates(value); !ok) {
            return ok;
        }
        break;
    }
    case Tournament::ReportField::Player: {
        if (const auto player = readPlayer(line); !player) {
            return std::unexpected(player.error());
        }
        break;
    }
    default:
        break;
    }

    return {};
}

std::expected<void, QString> TRFReader::readDates(QStringView line)
{
    if (line.length() < 88) {
        return {};
    }

    QStringView dateString;
    for (qsizetype i = 0;; ++i) {
        dateString = line.mid(87 + (10 * i), 8);

        if (dateString.isEmpty()) {
            break;
        }
        if (dateString.trimmed().isEmpty()) {
            continue;
        }

        const auto date = QDateTime::fromString(dateString.trimmed(), trfDateFormat);
        if (!date.isValid()) {
            return std::unexpected(i18nc("@info", "Date \"%1\" is invalid.", dateString.toString()));
        }

        if (const auto ok = m_tournament->ensureRoundExists(i + 1); !ok) {
            return ok;
        }

        const auto round = m_tournament->round(i + 1);
        round->setDateTime(date);

        if (const auto ok = m_tournament->saveRound(round); !ok) {
            return ok;
        }
    }

    return {};
}

std::expected<void, QString> TRFReader::readPlayer(QStringView line)
{
    const auto startingRank = line.sliced(4, 4).toInt();
    const auto sex = line.sliced(9, 1).trimmed().toString();
    const auto title = Player::titleForString(line.sliced(10, 3).trimmed().toString());
    const auto name = line.sliced(14, 33).trimmed().toString();
    const auto rating = line.sliced(48, 4).toInt();
    const auto federation = line.sliced(53, 5).trimmed().toString();
    const auto playerId = line.sliced(57, 11).trimmed().toString();
    const auto birthDate = line.sliced(69, 10).trimmed().toString();

    const auto player = new Player(startingRank, title, name, rating, 0, playerId, birthDate, federation, {}, sex);
    m_players[startingRank] = player;

    // Read round
    const auto playerRounds = line.mid(91);
    QStringView round;

    for (qsizetype roundNumber = 1;; roundNumber++) {
        round = playerRounds.mid(10 * (roundNumber - 1), 8);

        if (round.isEmpty() || round == "        "_L1) {
            break;
        }

        if (const auto pairing = readPairing(startingRank, roundNumber, round); !pairing) {
            return pairing;
        }
    }

    return {};
}

std::expected<void, QString> TRFReader::readPairing(int startingRank, int round, QStringView text)
{
    if (text.size() != 8) {
        return std::unexpected(i18n("Invalid pairing \"%1\".", text.toString()));
    }

    TRFReader::pairing pairing;

    const auto opponent = text.first(4);
    const bool hasOpponent = !(opponent == "    "_L1 || opponent == "0000"_L1);
    int opponentId = 0;

    if (hasOpponent) {
        bool ok;
        opponentId = opponent.toInt(&ok);
        if (!ok || opponentId <= 0) {
            return std::unexpected(i18n("Invalid player \"%1\" on pairing \"%2\".", QString::number(opponentId), text.toString()));
        }
    }

    auto color = Pairing::colorForString(text.at(5));
    auto result = Pairing::partialResultForTRF(text.at(7));
    if (result == Pairing::PartialResult::Unknown) {
        return std::unexpected(i18n("Unknown result \"%1\" on pairing \"%2\".", text.at(7), text.toString()));
    }
    if (!hasOpponent && result == Pairing::PartialResult::LostForfeit) {
        // Lichess exports zero point byes as forfeit loss with no opponent.
        result = Pairing::PartialResult::ZeroBye;
    }
    if (!hasOpponent && !Pairing::isBye(result)) {
        return std::unexpected(i18n("Pairing \"%1\" has no opponent.", text.toString()));
    }

    if (color == Pairing::Color::White || !hasOpponent) {
        pairing = {.round = round, .white = startingRank, .black = opponentId};
    } else {
        pairing = {.round = round, .white = opponentId, .black = startingRank};
    }

    auto newPairing = m_pairings[pairing];
    if (color == Pairing::Color::White || !hasOpponent) {
        newPairing.first = result;
    } else {
        newPairing.second = result;
    }
    m_pairings[pairing] = newPairing;

    return {};
}
