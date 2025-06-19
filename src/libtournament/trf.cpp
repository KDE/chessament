// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tournament.h"

#include <KLocalizedString>

std::expected<bool, QString> Tournament::readTrf(QTextStream trf)
{
    QMap<int, Player *> players = {};
    QMap<std::tuple<int, int, int>, Pairing::Result> pairingsToAdd = {};

    while (!trf.atEnd()) {
        auto line = trf.readLine();

        if (line.isEmpty()) {
            continue;
        }

        auto fieldType = line.mid(0, 3);
        auto field = Tournament::reportFieldForString(fieldType);

        if (field == Tournament::ReportField::TournamentName) {
            auto name = line.mid(4).trimmed();
            setName(name);
        } else if (field == Tournament::ReportField::City) {
            auto city = line.mid(4).trimmed();
            setCity(city);
        } else if (field == Tournament::ReportField::Federation) {
            auto federation = line.mid(4).trimmed();
            setFederation(federation);
        } else if (field == Tournament::ReportField::ChiefArbiter) {
            auto chiefArbiter = line.mid(4).trimmed();
            setChiefArbiter(chiefArbiter);
        } else if (field == Tournament::ReportField::DeputyChiefArbiter) {
            auto deputyChiefArbiter = line.mid(4).trimmed();
            setDeputyChiefArbiter(deputyChiefArbiter);
        } else if (field == Tournament::ReportField::TimeControl) {
            auto timeControl = line.mid(4).trimmed();
            setTimeControl(timeControl);
        } else if (field == Tournament::ReportField::Player) {
            auto startingRank = line.sliced(4, 4).toInt();
            auto sex = line.sliced(9, 1).trimmed();
            auto title = Player::titleForString(line.sliced(10, 3).trimmed());
            auto name = line.sliced(14, 33).trimmed();
            auto rating = line.sliced(48, 4).toInt();
            auto federation = line.sliced(53, 5).trimmed();
            auto playerId = line.sliced(57, 11).trimmed();
            auto birthDate = line.sliced(69, 10).trimmed();

            QString surname;
            if (name.contains(','_L1)) {
                const auto parts = name.split(u","_s);
                if (parts.length() == 2) {
                    surname = parts[0];
                    name = parts[1];
                }
            }

            auto player = new Player(startingRank, title, name, surname, rating, 0, playerId, birthDate, federation, {}, sex);
            players[startingRank] = player;

            // Read round
            auto playerRounds = line.mid(91);
            QString round;

            for (int roundNumber = 1;; roundNumber++) {
                round = playerRounds.mid(10 * (roundNumber - 1), 8);
                if (round.isEmpty() || round == u"        "_s) {
                    break;
                }

                if (round.size() != 8) {
                    return std::unexpected(i18n("Invalid pairing \"%1\".", round));
                }

                std::tuple<int, int, int> pairing{};

                auto opponent = round.first(4);
                int opponentId = 0;
                bool hasOpponent = !(opponent == u"    "_s || opponent == u"0000"_s);

                if (hasOpponent) {
                    bool ok;
                    opponentId = opponent.toInt(&ok);
                    if (!ok || opponentId <= 0) {
                        return std::unexpected(i18n("Invalid player \"%1\" on pairing \"%2\".", QString::number(opponentId), round));
                    }
                }

                auto color = Pairing::colorForString(round.at(5));
                auto result = Pairing::partialResultForTRF(round.at(7));
                if (result == Pairing::PartialResult::Unknown) {
                    return std::unexpected(i18n("Unknown result \"%1\" on pairing \"%2\".", round.at(7), round));
                }
                if (!hasOpponent && !Pairing::isBye(result)) {
                    return std::unexpected(i18n("Pairing \"%1\" has no opponent.", round));
                }

                if (color == Pairing::Color::White || !hasOpponent) {
                    pairing = {roundNumber, startingRank, opponentId};
                } else {
                    pairing = {roundNumber, opponentId, startingRank};
                }

                auto newPairing = pairingsToAdd[pairing];
                if (color == Pairing::Color::White || !hasOpponent) {
                    newPairing.first = result;
                } else {
                    newPairing.second = result;
                }
                pairingsToAdd[pairing] = newPairing;
            }
        }
    }

    for (const auto &player : std::as_const(players)) {
        addPlayer(player);
    }

    for (auto pairing = pairingsToAdd.cbegin(), end = pairingsToAdd.cend(); pairing != end; ++pairing) {
        const auto [r, w, b] = pairing.key();

        if (!players.contains(w)) {
            return std::unexpected(i18n("Player \"%1\" not found.", w));
        }
        auto whitePlayer = players.value(w);

        Player *blackPlayer = nullptr;
        if (b != 0) {
            if (!players.contains(b)) {
                return std::unexpected(i18n("Player \"%1\" not found.", b));
            }
            blackPlayer = players.value(b);
        }

        if (pairing.value().first == Pairing::PartialResult::Unknown && pairing.value().second == Pairing::PartialResult::Unknown) {
            return std::unexpected(i18n("Unknown result on pairing \"%1\" with \"%2\".", QString::number(w), QString::number(b)));
        }
        auto par = new Pairing(1, whitePlayer, blackPlayer, pairing.value().first, pairing.value().second);

        addPairing(r, par);
    }

    setNumberOfRounds(m_rounds.size());

    sortPairings();

    setCurrentRound(m_rounds.size());
    for (int i = 1; i <= m_numberOfRounds; ++i) {
        if (!isRoundFullyPaired(i)) {
            setCurrentRound(i - 1);
            break;
        }
    }

    if (m_currentRound > 0) {
        Tournament::InitialColor color;
        const auto pairing = getPairings(1).constFirst();
        if (pairing->whitePlayer()->startingRank() < pairing->blackPlayer()->startingRank()) {
            color = Tournament::InitialColor::White;
        } else {
            color = Tournament::InitialColor::Black;
        }
        setInitialColor(color);
    }

    return true;
}
