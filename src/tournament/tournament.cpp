// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tournament.h"

#include <KLocalizedString>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <algorithm>

#include "db.h"
#include "event.h"
#include "state.h"
#include "tiebreaks/aob.h"
#include "tiebreaks/buchholz.h"
#include "tiebreaks/numberwins.h"
#include "tiebreaks/playedblack.h"
#include "tiebreaks/points.h"
#include "tiebreaks/won.h"
#include "trf/reader.h"
#include "trf/writer.h"

Tournament::Tournament(Event *event)
    : m_event(event)
{
    m_tiebreaks.push_back(std::make_unique<Points>());
}

QString Tournament::id() const
{
    return m_id;
}

void Tournament::setId(const QString &id)
{
    if (m_id == id) {
        return;
    }
    m_id = id;
    Q_EMIT idChanged();
}

QString Tournament::name() const
{
    return m_name;
}

void Tournament::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }
    m_name = name;
    setOption(u"name"_s, name);
    Q_EMIT nameChanged();
}

QString Tournament::city() const
{
    return m_city;
}

void Tournament::setCity(const QString &city)
{
    if (m_city == city) {
        return;
    }
    m_city = city;
    setOption(u"city"_s, city);
    Q_EMIT cityChanged();
}

QString Tournament::federation() const
{
    return m_federation;
}

void Tournament::setFederation(const QString &federation)
{
    if (m_federation == federation) {
        return;
    }
    m_federation = federation;
    setOption(u"federation"_s, federation);
    Q_EMIT federationChanged();
}

QString Tournament::chiefArbiter() const
{
    return m_chiefArbiter;
}

void Tournament::setChiefArbiter(const QString &chiefArbiter)
{
    if (m_chiefArbiter == chiefArbiter) {
        return;
    }
    m_chiefArbiter = chiefArbiter;
    setOption(u"chief_arbiter"_s, chiefArbiter);
    Q_EMIT chiefArbiterChanged();
}

QString Tournament::deputyChiefArbiter() const
{
    return m_deputyChiefArbiter;
}

void Tournament::setDeputyChiefArbiter(const QString &deputyChiefArbiter)
{
    if (m_deputyChiefArbiter == deputyChiefArbiter) {
        return;
    }
    m_deputyChiefArbiter = deputyChiefArbiter;
    setOption(u"deputy_chief_arbiter"_s, deputyChiefArbiter);
    Q_EMIT deputyChiefArbiterChanged();
}

QString Tournament::timeControl() const
{
    return m_timeControl;
}

void Tournament::setTimeControl(const QString &timeControl)
{
    if (m_timeControl == timeControl) {
        return;
    }
    m_timeControl = timeControl;
    setOption(u"time_control"_s, timeControl);
    Q_EMIT timeControlChanged();
}

std::vector<std::unique_ptr<Tiebreak>> &Tournament::tiebreaks()
{
    return m_tiebreaks;
}

void Tournament::setTiebreaks(std::vector<std::unique_ptr<Tiebreak>> tiebreaks)
{
    if (m_tiebreaks == tiebreaks) {
        return;
    }
    m_tiebreaks = std::move(tiebreaks);
    saveTiebreaks();
}

std::expected<void, QString> Tournament::setTiebreaksFromTrf(const QString &line)
{
    std::vector<std::unique_ptr<Tiebreak>> tiebreaks;

    const auto codes = line.split(u',', Qt::SkipEmptyParts);

    for (const auto &code : codes) {
        auto tiebreak = tiebreakFromTrf(code);
        if (!tiebreak) {
            return std::unexpected(tiebreak.error());
        }
        if (tiebreak == nullptr) {
            continue;
        }
        tiebreaks.push_back(std::move(*tiebreak));
    }

    setTiebreaks(std::move(tiebreaks));

    return {};
}

int Tournament::numberOfRounds() const
{
    return m_numberOfRounds;
}

void Tournament::setNumberOfRounds(int numberOfRounds)
{
    if (m_numberOfRounds == numberOfRounds) {
        return;
    }
    m_numberOfRounds = numberOfRounds;
    setOption(u"number_of_rounds"_s, numberOfRounds);
    Q_EMIT numberOfRoundsChanged();
}

int Tournament::currentRound() const
{
    return m_currentRound;
}

void Tournament::setCurrentRound(int currentRound)
{
    Q_ASSERT(currentRound >= 0);
    if (m_currentRound == currentRound) {
        return;
    }
    m_currentRound = currentRound;
    setOption(u"current_round"_s, currentRound);
    Q_EMIT currentRoundChanged();
}

QList<Player *> Tournament::players() const
{
    QList<Player *> result;
    result.reserve(static_cast<qsizetype>(m_players.size()));

    for (const auto &player : std::as_const(m_players)) {
        result << player.get();
    }

    return result;
}

std::expected<void, QString> Tournament::addPlayer(std::unique_ptr<Player> player)
{
    m_event->db().transaction();

    QSqlQuery query(m_event->db());
    query.prepare(ADD_PLAYER_QUERY);
    query.bindValue(u":startingRank"_s, player->startingRank());
    query.bindValue(u":title"_s, player->title());
    query.bindValue(u":name"_s, player->name());
    query.bindValue(u":rating"_s, player->rating());
    query.bindValue(u":nationalRating"_s, player->nationalRating());
    query.bindValue(u":playerId"_s, player->playerId());
    query.bindValue(u":birthDate"_s, player->birthDate());
    query.bindValue(u":federation"_s, player->federation());
    query.bindValue(u":origin"_s, player->origin());
    query.bindValue(u":sex"_s, player->sex());
    query.bindValue(u":extra"_s, player->extraString());
    query.bindValue(u":tournament"_s, m_id);

    if (!query.exec()) {
        qDebug() << "add player" << *player << query.lastError();
        return std::unexpected(query.lastError().text());
    }

    player->setId(query.lastInsertId().toInt());

    std::vector<std::unique_ptr<Pairing>> pairings;
    for (int i = 1; i <= m_currentRound; ++i) {
        const qsizetype board = this->pairings(i).size() + 1;

        auto pairing = std::make_unique<Pairing>(board, player.get(), nullptr, Pairing::PartialResult::ZeroBye, Pairing::PartialResult::Unknown);
        if (auto ok = savePairing(pairing.get(), i + 1); !ok) {
            return std::unexpected(ok.error());
        }

        pairings.push_back(std::move(pairing));
    }

    if (!m_event->db().commit()) {
        return std::unexpected(m_event->db().lastError().text());
    }

    m_players.push_back(std::move(player));

    for (int i = 1; i <= m_currentRound; ++i) {
        const auto round = this->round(i);
        auto pairing = std::move(pairings.at(i - 1));
        round->addPairing(std::move(pairing));
    }

    Q_EMIT numberOfPlayersChanged();
    Q_EMIT numberOfRatedPlayersChanged();

    return {};
}

void Tournament::savePlayer(Player *player)
{
    QSqlQuery query(m_event->db());
    query.prepare(UPDATE_PLAYER_QUERY);
    query.bindValue(u":id"_s, player->id());
    query.bindValue(u":startingRank"_s, player->startingRank());
    query.bindValue(u":title"_s, player->title());
    query.bindValue(u":name"_s, player->name());
    query.bindValue(u":rating"_s, player->rating());
    query.bindValue(u":nationalRating"_s, player->nationalRating());
    query.bindValue(u":playerId"_s, player->playerId());
    query.bindValue(u":birthDate"_s, player->birthDate());
    query.bindValue(u":federation"_s, player->federation());
    query.bindValue(u":origin"_s, player->origin());
    query.bindValue(u":sex"_s, player->sex());
    query.bindValue(u":extra"_s, player->extraString());
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "save player" << *player << query.lastError();
    }

    Q_EMIT numberOfRatedPlayersChanged();
}

void Tournament::sortPlayers()
{
    std::ranges::sort(m_players, [](const std::unique_ptr<Player> &p1, const std::unique_ptr<Player> &p2) {
        if (p1->rating() == p2->rating()) {
            if (Player::titleStrengthLevel(p1->title()) == Player::titleStrengthLevel(p2->title())) {
                auto cmp = p1->name().toLower().localeAwareCompare(p2->name().toLower());
                return cmp < 0;
            }
            return Player::titleStrengthLevel(p1->title()) < Player::titleStrengthLevel(p2->title());
        }
        return p1->rating() > p2->rating();
    });

    for (std::size_t i = 0; i < m_players.size(); i++) {
        auto *player = m_players.at(i).get();
        player->setStartingRank(i + 1);
        savePlayer(player);
    }
}

void Tournament::changePlayerStartingRank(Player *player, int startingRank)
{
    Q_ASSERT(startingRank >= 1);

    const auto rank = std::min(startingRank, numberOfPlayers());

    const auto &players = m_players;
    for (const auto &p : players) {
        if (p->startingRank() >= rank && p->startingRank() < player->startingRank()) {
            p->setStartingRank(p->startingRank() + 1);
            savePlayer(p.get());
        } else if (p->startingRank() > player->startingRank() && p->startingRank() <= rank) {
            p->setStartingRank(p->startingRank() - 1);
            savePlayer(p.get());
        }
    }

    player->setStartingRank(rank);
    savePlayer(player);
}

QMap<uint, Player *> Tournament::playersByStartingRank()
{
    QMap<uint, Player *> players;

    for (const auto &player : std::as_const(m_players)) {
        players[player->startingRank()] = player.get();
    }

    return players;
}

QMap<uint, Player *> Tournament::playersById()
{
    QMap<uint, Player *> players;

    for (const auto &player : std::as_const(m_players)) {
        players[player->id()] = player.get();
    }

    return players;
}

QHash<Player *, QList<Pairing *>> Tournament::pairingsByPlayer(int maxRound)
{
    QHash<Player *, QList<Pairing *>> pairings;

    auto r = maxRound < 0 ? m_rounds.size() : maxRound;
    for (std::size_t i = 0; i < r; i++) {
        if (i >= m_rounds.size()) {
            break;
        }
        const auto round = m_rounds.at(i).get();
        for (const auto &pairing : round->pairings()) {
            pairings[pairing->whitePlayer()] << pairing;
            if (pairing->blackPlayer() != nullptr) {
                pairings[pairing->blackPlayer()] << pairing;
            }
        }
    }

    return pairings;
}

QList<Pairing *> Tournament::pairingsOfPlayer(Player *player)
{
    QList<Pairing *> result;

    for (size_t i = 0; i < m_numberOfRounds; ++i) {
        if (i >= m_rounds.size()) {
            result << nullptr;
            continue;
        }

        Pairing *p = nullptr;
        const auto &round = m_rounds.at(i);
        for (const auto &pairing : round->pairings()) {
            if (pairing->whitePlayer() == player || pairing->blackPlayer() == player) {
                p = pairing;
            }
        }
        result << p;
    }

    return result;
}

QList<Standing> Tournament::standings(const State &state)
{
    QList<Standing> standings;

    // Sort by tiebreaks
    auto sortStandings = [&standings]() {
        std::ranges::sort(standings, [](const Standing &p1, const Standing &p2) {
            for (size_t i = 0; i < p1.values().size(); i++) {
                if (p1.values().at(i) == p2.values().at(i)) {
                    continue;
                }
                return p1.values().at(i) > p2.values().at(i);
            }
            return p1.player()->startingRank() < p2.player()->startingRank();
        });
    };

    for (const auto &player : std::as_const(m_players)) {
        standings << Standing(player.get(), {});
    }

    // Calculate tiebreaks
    QList<Player *> players;
    for (const auto &tiebreak : std::as_const(m_tiebreaks)) {
        uint i = 0;
        players.clear();
        while (i < m_players.size()) {
            if (players.isEmpty()) {
                players << standings.at(i).player();
                i++;
                continue;
            }
            if (standings.at(i - 1).values() == standings.at(i).values()) {
                players << standings.at(i).player();
                i++;
            } else {
                for (int j = 0; j < players.size(); j++) {
                    /*for (const auto &p : players) {
                        qDebug() << p->name();
                    }
                    qDebug() << "Player" << j << "is" << players.at(j)->name();
                    qDebug() << "SPlayer" << j << "is" << standings.at(i - players.size() + j).first->name();
                    qDebug() << "--";*/
                    standings[i - players.size() + j].addValue(tiebreak->calculate(this, state, players, players.at(j)));
                }
                players.clear();
                players << standings.at(i).player();
                i++;
            }
        }
        for (int j = 0; j < players.size(); j++) {
            /*for (const auto &p : players) {
                qDebug() << p->name();
            }
            qDebug() << "Player" << j << "is" << players.at(j)->name();
            qDebug() << "SPlayer" << j << "is" << standings.at(i - players.size() + j).first->name();
            qDebug() << "--";*/
            standings[i - players.size() + j].addValue(tiebreak->calculate(this, state, players, players.at(j)));
        }
        sortStandings();
    }

    // Calculate ranks
    for (int i = 1; i < standings.size(); ++i) {
        auto &standing = standings[i];
        if (standing.values() == standings.at(i - 1).values()) {
            standing.setRank(standings.at(i - 1).rank());
        } else {
            standing.setRank(i + 1);
        }
    }

    return standings;
}

QList<QVariantMap> Tournament::availableTiebreaks()
{
    return {
        {
            {"id"_L1, "pts"_L1},
            {"name"_L1, i18nc("Game points", "Points")},
        },
        {
            {"id"_L1, "bh"_L1},
            {"name"_L1, i18nc("Buchholz tiebreak", "Buchholz")},
        },
        {
            {"id"_L1, "win"_L1},
            {"name"_L1, i18nc("Number of Wins tiebreak", "Number of Wins")},
        },
        {
            {"id"_L1, "won"_L1},
            {"name"_L1,
             i18nc("Number of Games Won (over the board) tiebreak. Different from the 'Number of Wins' tiebreak, which includes games won by forfeit.",
                   "Number of Games Won (over the board)")},
        },
        {
            {"id"_L1, "bpg"_L1},
            {"name"_L1, i18nc("Number of Games Played with Black (over the board) tiebreak", "Number of Games Played with Black (over the board)")},
        },
        {
            {"id"_L1, "aob"_L1},
            {"name"_L1, i18nc("Tiebreak", "Average Buchholz of Opponents")},
        },
    };
}

std::unique_ptr<Tiebreak> Tournament::tiebreak(const QString &id)
{
    if (id == "pts"_L1) {
        return std::make_unique<Points>();
    }
    if (id == "bh"_L1) {
        return std::make_unique<Buchholz>();
    }
    if (id == "win"_L1) {
        return std::make_unique<NumberOfWins>();
    }
    if (id == "won"_L1) {
        return std::make_unique<NumberOfGamesWon>();
    }
    if (id == "bpg"_L1) {
        return std::make_unique<NumberOfGamesPlayedWithBlack>();
    }
    if (id == "aob"_L1) {
        return std::make_unique<AverageBuchholzOfOpponents>();
    }
    return nullptr;
}

std::expected<std::unique_ptr<Tiebreak>, QString> Tournament::tiebreakFromTrf(const QString &code)
{
    if (code.startsWith("OTHER_"_L1, Qt::CaseSensitivity::CaseInsensitive)) {
        qWarning() << "Unsupported tiebreak" << code;
        return nullptr;
    }

    const auto options = code.split(u'/', Qt::SkipEmptyParts);

    auto tiebreak = Tournament::tiebreak(options[0].toLower());
    if (tiebreak == nullptr) {
        qWarning() << "Unsupported tiebreak" << code;
        return nullptr;
    }

    if (const auto ok = tiebreak->setTrfOptions(options.mid(1)); !ok) {
        return std::unexpected(ok.error());
    }

    return tiebreak;
}

void Tournament::setInitialColor(Tournament::InitialColor color)
{
    if (m_initialColor == color) {
        return;
    }
    m_initialColor = color;
    setOption(u"initial_color"_s, std::to_underlying(color));
}

Round *Tournament::round(int number) const
{
    Q_ASSERT(number >= 1);

    if (static_cast<size_t>(number) > m_rounds.size()) {
        return nullptr;
    }
    return m_rounds[number - 1].get();
}

std::expected<void, QString> Tournament::ensureRoundExists(int round)
{
    Q_ASSERT(round >= 1);

    if (m_rounds.size() < static_cast<size_t>(round)) {
        for (size_t i = m_rounds.size() + 1; i <= static_cast<size_t>(round); ++i) {
            auto round = std::make_unique<Round>();

            QSqlQuery query(m_event->db());
            query.prepare(ADD_ROUND_QUERY);
            query.bindValue(u":number"_s, static_cast<qulonglong>(i));
            query.bindValue(u":tournament"_s, m_id);
            query.bindValue(u":extra"_s, round->extraString());
            query.exec();

            if (query.lastError().isValid()) {
                return std::unexpected(query.lastError().text());
            }

            round->setId(query.lastInsertId().toInt());
            m_rounds.push_back(std::move(round));
        }
    }

    return {};
}

std::expected<void, QString> Tournament::saveRound(Round *round)
{
    const auto dateTime = round->dateTime().toUTC().toString(Qt::ISODate);

    QSqlQuery query(m_event->db());
    query.prepare(UPDATE_ROUND_QUERY);
    query.bindValue(u":id"_s, round->id());
    query.bindValue(u":datetime"_s, dateTime);
    query.bindValue(u":extra"_s, round->extraString());
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "save round" << query.lastError();
        return std::unexpected(query.lastError().text());
    }

    return {};
}

std::expected<void, QString> Tournament::addPairing(int roundNumber, std::unique_ptr<Pairing> pairing)
{
    if (const auto ok = savePairing(pairing.get(), roundNumber); !ok) {
        return ok;
    }

    const auto round = this->round(roundNumber);
    round->addPairing(std::move(pairing));

    return {};
}

std::expected<void, QString> Tournament::savePairing(Pairing *pairing, int roundNumber)
{
    QSqlQuery query(m_event->db());

    if (pairing->id().isEmpty()) {
        Q_ASSERT(roundNumber >= 1);

        if (const auto ok = ensureRoundExists(roundNumber); !ok) {
            return ok;
        }

        const auto round = this->round(roundNumber);

        pairing->setId(QUuid::createUuid().toString(QUuid::WithoutBraces));

        query.prepare(ADD_PAIRING_QUERY);
        query.bindValue(u":round"_s, round->id());
    } else {
        query.prepare(UPDATE_PAIRING_QUERY);
    }

    query.bindValue(u":id"_s, pairing->id());
    query.bindValue(u":board"_s, pairing->board());
    query.bindValue(u":whitePlayer"_s, pairing->whitePlayer()->id());
    if (pairing->blackPlayer() != nullptr) {
        query.bindValue(u":blackPlayer"_s, pairing->blackPlayer()->id());
    } else {
        query.bindValue(u":blackPlayer"_s, QVariant(QMetaType::fromType<int>()));
    }
    query.bindValue(u":whiteResult"_s, std::to_underlying(pairing->whiteResult()));
    query.bindValue(u":blackResult"_s, std::to_underlying(pairing->blackResult()));
    query.bindValue(u":extra"_s, pairing->extraString());
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "save pairing" << query.lastError();
        return std::unexpected(query.lastError().text());
    }

    return {};
}

std::expected<void, QString> Tournament::setResult(Pairing *pairing, Pairing::Result result)
{
    pairing->setResult(result);

    if (auto ok = savePairing(pairing); !ok) {
        return ok;
    }

    return {};
}

std::expected<void, QString> Tournament::setBye(Player *player, int round, Pairing::PartialResult result)
{
    Q_ASSERT(round > m_currentRound);

    const auto pairing = this->pairing(round, player);

    if (pairing == nullptr && result == Pairing::PartialResult::Unknown) {
        return {};
    }

    if (pairing == nullptr) {
        const qsizetype board = pairings(round).size() + 1;
        auto p = std::make_unique<Pairing>(board, player, nullptr, result, Pairing::PartialResult::Unknown);

        return addPairing(round, std::move(p));
    }

    if (result == Pairing::PartialResult::Unknown) {
        return removePairing(round, pairing);
    }

    pairing->setWhiteResult(result);

    return savePairing(pairing);
}

std::expected<void, QString> Tournament::retire(Player *player)
{
    for (int i = m_currentRound + 1; i <= m_numberOfRounds; ++i) {
        if (const auto ok = setBye(player, i, Pairing::PartialResult::ZeroBye); !ok) {
            return ok;
        }
    }

    return {};
}

QList<Player *> Tournament::voluntaryByes(int round) const
{
    Q_ASSERT(round >= 1);

    QList<Player *> result{};

    for (const auto p : pairings(round)) {
        if (Pairing::isVoluntaryBye(p->whiteResult())) {
            result << p->whitePlayer();
        }
    }

    return result;
}

Pairing *Tournament::pairing(int round, int board)
{
    Q_ASSERT(round >= 1);
    Q_ASSERT(static_cast<std::size_t>(round) <= m_rounds.size());
    Q_ASSERT(board >= 1);

    return m_rounds.at(round - 1)->pairing(board);
}

QList<Pairing *> Tournament::pairings(int round) const
{
    if (static_cast<std::size_t>(round) <= m_rounds.size()) {
        return m_rounds.at(round - 1)->pairings();
    }

    return {};
}

Pairing *Tournament::pairing(int round, Player *player) const
{
    const auto all = pairings(round);

    for (const auto &p : all) {
        if (p->whitePlayer() == player || p->blackPlayer() == player) {
            return p;
        }
    }

    return nullptr;
}

int Tournament::numberOfPlayers()
{
    return m_players.size();
}

int Tournament::numberOfRatedPlayers()
{
    return std::count_if(m_players.cbegin(), m_players.cend(), [](const auto &p) {
        return p->rating() > 0;
    });
}

std::expected<void, QString> Tournament::sortPairings(int round)
{
    Q_ASSERT(round != 0);

    const auto firstRound = round < 0 ? 0 : round - 1;
    const auto lastRound = round < 0 ? m_rounds.size() : round;

    for (size_t i = firstRound; i < lastRound; i++) {
        auto &pairings = m_rounds[i]->m_pairings;

        const auto state = this->state(static_cast<int>(i));

        std::ranges::sort(pairings, [i, &state](const std::unique_ptr<Pairing> &a, const std::unique_ptr<Pairing> &b) -> bool {
            int aRank;
            if (a->blackPlayer() == nullptr) {
                aRank = 0;
            } else {
                aRank = std::min(a->whitePlayer()->startingRank(), a->blackPlayer()->startingRank());
            }

            int bRank;
            if (b->blackPlayer() == nullptr) {
                bRank = 0;
            } else {
                bRank = std::min(b->whitePlayer()->startingRank(), b->blackPlayer()->startingRank());
            }

            if (aRank == 0 && bRank == 0) {
                if (std::to_underlying(a->whiteResult()) == std::to_underlying(b->whiteResult())) {
                    return a->whitePlayer()->startingRank() < b->whitePlayer()->startingRank();
                }
                return std::to_underlying(a->whiteResult()) > std::to_underlying(b->whiteResult());
            }
            if (aRank == 0) {
                return false;
            }
            if (bRank == 0) {
                return true;
            }

            double aScore;
            double aTotal;
            if (a->whitePlayer()->startingRank() < a->blackPlayer()->startingRank()) {
                aScore = state.points(a->whitePlayer());
                aTotal = aScore + state.points(a->blackPlayer());
            } else {
                aScore = state.points(a->blackPlayer());
                aTotal = aScore + state.points(a->whitePlayer());
            }
            double bScore;
            double bTotal;
            if (b->whitePlayer()->startingRank() < b->blackPlayer()->startingRank()) {
                bScore = state.points(b->whitePlayer());
                bTotal = bScore + state.points(b->blackPlayer());
            } else {
                bScore = state.points(b->blackPlayer());
                bTotal = bScore + state.points(b->whitePlayer());
            }

            if (i > 0) {
                if (aScore != bScore) {
                    return aScore > bScore;
                }

                //
                if (aTotal != bTotal) {
                    return aTotal > bTotal;
                }
            }

            return aRank < bRank;
        });

        for (size_t i = 0; i < pairings.size(); i++) {
            pairings.at(i)->setBoard(static_cast<int>(i) + 1);

            if (auto ok = savePairing(pairings[i].get()); !ok) {
                return ok;
            }
        }

        // m_rounds.at(i)->setPairings(pairings);
    }

    return {};
}

bool Tournament::isRoundFinished(int round)
{
    qDebug() << "is round finished" << round;
    Q_ASSERT(round >= 1);

    QList<Pairing *> pairings;

    if (static_cast<std::size_t>(round) <= m_rounds.size()) {
        pairings = m_rounds.at(round - 1)->pairings();
    } else {
        return false; // TODO: ok?
    }

    auto finished = std::count_if(pairings.cbegin(), pairings.cend(), [](Pairing *p) {
        return Pairing::isBye(p->whiteResult()) || (p->whiteResult() != Pairing::PartialResult::Unknown && p->blackResult() != Pairing::PartialResult::Unknown);
    });

    return finished == pairings.size();
}

bool Tournament::isRoundFullyPaired(int round)
{
    Q_ASSERT(round >= 1);
    Q_ASSERT(static_cast<std::size_t>(round) <= m_rounds.size());

    const auto pairings = m_rounds.at(round - 1)->pairings();

    QSet<Player *> players;

    for (const auto &pairing : pairings) {
        players.insert(pairing->whitePlayer());
        if (pairing->blackPlayer() != nullptr) {
            players.insert(pairing->blackPlayer());
        }
    }

    return static_cast<qsizetype>(m_players.size()) == players.size();
}

QCoro::Task<std::expected<QList<std::pair<uint, uint>>, QString>> Tournament::calculatePairings(int round)
{
    auto engine = std::make_unique<PairingEngine>();
    const auto pairings = co_await engine->pair(round, this);

    if (!pairings.has_value()) {
        co_return std::unexpected(pairings.error());
    }

    co_return pairings;
}

QCoro::Task<std::expected<bool, QString>> Tournament::pairNextRound()
{
    const auto roundToPair = m_currentRound + 1;
    const auto pairings = co_await calculatePairings(roundToPair);

    if (!pairings.has_value()) {
        co_return std::unexpected(i18n("An error ocurred while pairing the round: %1", pairings.error()));
    }

    const auto players = playersByStartingRank();

    uint board = 1;
    for (const auto &pairing : *pairings) {
        const auto &whitePlayer = players.value(pairing.first);
        Player *blackPlayer = nullptr;
        Pairing::PartialResult whiteResult = Pairing::PartialResult::PairingBye;

        if (pairing.second != 0) {
            blackPlayer = players.value(pairing.second);
            whiteResult = Pairing::PartialResult::Unknown;
        }

        auto p = std::make_unique<Pairing>(board, whitePlayer, blackPlayer, whiteResult, Pairing::PartialResult::Unknown);
        if (auto ok = addPairing(roundToPair, std::move(p)); !ok) {
            co_return std::unexpected(ok.error());
        }

        board++;
    }

    if (auto ok = sortPairings(roundToPair); !ok) {
        co_return std::unexpected(ok.error());
    }

    setCurrentRound(roundToPair);

    co_return true;
}

std::expected<void, QString> Tournament::removePairing(int round, Pairing *pairing)
{
    Q_ASSERT(round >= 1);
    Q_ASSERT(pairing != nullptr);
    Q_ASSERT(!pairing->id().isEmpty());

    QSqlQuery query(m_event->db());
    query.prepare(DELETE_PAIRING_QUERY);
    query.bindValue(":id"_L1, pairing->id());

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    m_rounds.at(round - 1)->removePairings([&pairing](Pairing *p) {
        return p->id() == pairing->id();
    });

    return {};
}

std::expected<void, QString> Tournament::removePairings(int round, bool keepByes)
{
    Q_ASSERT(round >= 1);
    Q_ASSERT(round <= m_numberOfRounds);

    for (size_t i = round; i <= m_rounds.size(); i++) {
        QSqlQuery query(m_event->db());
        if (keepByes) {
            query.prepare(DELETE_PAIRINGS_NO_BYES_QUERY);
        } else {
            query.prepare(DELETE_PAIRINGS_QUERY);
        }
        query.bindValue(u":round"_s, m_rounds.at(i - 1)->id());
        query.exec();

        if (query.lastError().isValid()) {
            qDebug() << "remove pairings" << query.lastError();
            return std::unexpected(query.lastError().text());
        }

        m_rounds.at(i - 1)->removePairings([keepByes](Pairing *pairing) {
            return !keepByes || !Pairing::isRequestedBye(pairing->whiteResult());
        });
    }

    setCurrentRound(round - 1);

    return {};
}

Tournament::InitialColor Tournament::initialColor()
{
    return m_initialColor;
}

State Tournament::state(int maxRound)
{
    return State{this, maxRound};
}

void Tournament::saveTiebreaks()
{
    QJsonArray values;

    for (const auto &tiebreak : tiebreaks()) {
        values << tiebreak->toJson();
    }

    const auto doc = QJsonDocument{QJsonObject{{"tiebreaks"_L1, values}}};
    const auto text = doc.toJson(QJsonDocument::Compact);

    setOption("tiebreaks"_L1, text);

    Q_EMIT tiebreaksChanged();
}

QVariant Tournament::option(const QString &name)
{
    QSqlQuery query(m_event->db());
    query.prepare(GET_OPTION_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.bindValue(u":name"_s, name);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "get option" << query.lastError();
    }

    query.next();
    if (!query.isValid()) {
        return {};
    }
    return query.value(0);
}

void Tournament::setOption(const QString &name, const QVariant &value)
{
    QSqlQuery query(m_event->db());
    query.prepare(UPDATE_OPTION_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.bindValue(u":name"_s, name);
    query.bindValue(u":value"_s, value);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "set option" << name << value << query.lastError();
    }
}

QJsonObject Tournament::toJson() const
{
    QJsonObject json;

    QJsonObject tournament;
    tournament[u"id"_s] = m_id;
    tournament[QStringLiteral("name")] = m_name;
    tournament[QStringLiteral("city")] = m_city;
    tournament[QStringLiteral("federation")] = m_federation;
    tournament[QStringLiteral("chief_arbiter")] = m_chiefArbiter;
    tournament[QStringLiteral("deputy_chief_arbiter")] = m_deputyChiefArbiter;
    tournament[QStringLiteral("time_control")] = m_timeControl;
    tournament[QStringLiteral("number_of_rounds")] = m_numberOfRounds;

    QJsonArray players;
    for (const auto &player : std::as_const(m_players)) {
        players << player->toJson();
    }

    json[QStringLiteral("tournament")] = tournament;
    json[QStringLiteral("players")] = players;

    return json;
}

void Tournament::read(const QJsonObject &json)
{
    if (auto v = json[QStringLiteral("tournament")]; v.isObject()) {
        auto tournament = v.toObject();

        if (const auto v = tournament[QStringLiteral("id")]; v.isString()) {
            m_id = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("name")]; v.isString()) {
            m_name = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("city")]; v.isString()) {
            m_city = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("federation")]; v.isString()) {
            m_federation = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("chief_arbiter")]; v.isString()) {
            m_chiefArbiter = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("deputy_chief_arbiter")]; v.isString()) {
            m_deputyChiefArbiter = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("time_control")]; v.isString()) {
            m_timeControl = v.toString();
        }
        if (const auto v = tournament[QStringLiteral("number_of_rounds")]; v.isDouble()) {
            m_numberOfRounds = v.toInt();
        }
    }

    if (auto v = json[QStringLiteral("players")]; v.isArray()) {
        auto players = v.toArray();
        m_players.clear();
        m_players.reserve(players.size());
        for (const auto &player : std::as_const(players)) {
            m_players.push_back(Player::fromJson(player.toObject()));
        }
    }
}

std::expected<void, QString> Tournament::readTrf(QTextStream trf)
{
    TrfReader reader{this};
    return reader.read(&trf);
}

QString Tournament::toTrf(Trf::Options options, int maxRound)
{
    QString result;
    QTextStream stream(&result);

    TrfWriter writer(this, options, maxRound);
    writer.write(stream);

    return result;
}

std::expected<void, QString> Tournament::loadTrf(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return std::unexpected(i18n("Couldn't open file"));
    }

    return readTrf(QTextStream(&file));
}

bool Tournament::exportTrf(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't export tournament report" << fileName;
        return false;
    }

    auto trf = toTrf();
    file.write(trf.toUtf8());

    return true;
}

std::expected<void, QString> Tournament::createNewTournament()
{
    Q_ASSERT(m_id.isEmpty());

    const auto newId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QSqlQuery query(m_event->db());
    query.prepare(ADD_TOURNAMENT_QUERY);
    query.bindValue(u":id"_s, newId);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "create tournament" << query.lastError();
        return std::unexpected(query.lastError().text());
    }

    setId(newId);

    return {};
}

std::expected<void, QString> Tournament::loadTournament(const QString &id)
{
    Q_ASSERT(m_id.isEmpty());

    setId(id);

    loadOptions();
    if (auto ok = loadPlayers(); !ok) {
        return ok;
    }
    if (auto ok = loadRounds(); !ok) {
        return ok;
    }
    if (auto ok = loadPairings(); !ok) {
        return ok;
    }
    if (auto ok = loadTiebreaks(); !ok) {
        return ok;
    }

    return {};
}

void Tournament::loadOptions()
{
    setName(option(u"name"_s).toString());
    setCity(option(u"city"_s).toString());
    setFederation(option(u"federation"_s).toString());
    setChiefArbiter(option(u"chief_arbiter"_s).toString());
    setDeputyChiefArbiter(option(u"deputy_chief_arbiter"_s).toString());
    setTimeControl(option(u"time_control"_s).toString());
    setNumberOfRounds(option(u"number_of_rounds"_s).toInt());
    setCurrentRound(option(u"current_round"_s).toInt());
    setInitialColor(Tournament::InitialColor(option(u"initial_color"_s).toInt()));
}

std::expected<void, QString> Tournament::loadPlayers()
{
    m_players.clear();

    QSqlQuery query(m_event->db());
    query.prepare(GET_PLAYERS_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    int idNo = query.record().indexOf("id");
    int stRankNo = query.record().indexOf("startingRank");
    int titleNo = query.record().indexOf("title");
    int nameNo = query.record().indexOf("name");
    int ratingNo = query.record().indexOf("rating");
    int nationalRatingNo = query.record().indexOf("nationalRating");
    int playerIdNo = query.record().indexOf("playerId");
    int birthDateNo = query.record().indexOf("birthDate");
    int federationNo = query.record().indexOf("federation");
    int originNo = query.record().indexOf("origin");
    int sexNo = query.record().indexOf("sex");
    int extraNo = query.record().indexOf("extra");

    while (query.next()) {
        auto player = std::make_unique<Player>(query.value(stRankNo).toInt(),
                                               query.value(titleNo).toString(),
                                               query.value(nameNo).toString(),
                                               query.value(ratingNo).toInt(),
                                               query.value(nationalRatingNo).toInt(),
                                               query.value(playerIdNo).toString(),
                                               query.value(birthDateNo).toString(),
                                               query.value(federationNo).toString(),
                                               query.value(originNo).toString(),
                                               query.value(sexNo).toString());
        player->setId(query.value(idNo).toInt());
        player->setExtra(query.value(extraNo).toByteArray());
        m_players.push_back(std::move(player));
    }

    return {};
}

std::expected<void, QString> Tournament::loadRounds()
{
    QSqlQuery query(m_event->db());
    query.prepare(GET_ROUNDS_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    int idNo = query.record().indexOf("id");
    int numberNo = query.record().indexOf("number");
    int dateTimeNo = query.record().indexOf("datetime");
    int extraNo = query.record().indexOf("extra");

    while (query.next()) {
        auto round = std::make_unique<Round>();
        round->setId(query.value(idNo).toInt());
        round->setNumber(query.value(numberNo).toInt());
        round->setDateTime(query.value(dateTimeNo).toDateTime());
        round->setExtra(query.value(extraNo).toByteArray());
        m_rounds.push_back(std::move(round));
    }

    std::ranges::sort(m_rounds, [](const std::unique_ptr<Round> &a, const std::unique_ptr<Round> &b) {
        return a->number() < b->number();
    });

    return {};
}

std::expected<void, QString> Tournament::loadPairings()
{
    auto players = playersById();

    QSqlQuery query(m_event->db());
    query.prepare(GET_PAIRINGS_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    int idNo = query.record().indexOf("id");
    int boardNo = query.record().indexOf("board");
    int whitePlayerNo = query.record().indexOf("whitePlayer");
    int blackPlayerNo = query.record().indexOf("blackPlayer");
    int whiteResultNo = query.record().indexOf("whiteResult");
    int blackResultNo = query.record().indexOf("blackResult");
    int roundNo = query.record().indexOf("round");

    while (query.next()) {
        auto round = query.value(roundNo).toInt();
        auto pairing = std::make_unique<Pairing>(query.value(boardNo).toInt(),
                                                 players.value(query.value(whitePlayerNo).toInt()),
                                                 players.value(query.value(blackPlayerNo).toInt()),
                                                 Pairing::PartialResult(query.value(whiteResultNo).toInt()),
                                                 Pairing::PartialResult(query.value(blackResultNo).toInt()));
        pairing->setId(query.value(idNo).toString());
        m_rounds.at(round - 1)->addPairing(std::move(pairing));
    }

    return {};
}

std::expected<void, QString> Tournament::loadTiebreaks()
{
    const auto json = QJsonDocument::fromJson(option("tiebreaks"_L1).toByteArray());

    if (const auto tbs = json["tiebreaks"_L1]; tbs.isArray()) {
        m_tiebreaks.clear();

        for (const auto value : tbs.toArray()) {
            const auto obj = value.toObject();
            const auto id = obj["id"_L1].toString();

            auto tiebreak = Tournament::tiebreak(id);
            if (tiebreak == nullptr) {
                continue;
            }

            if (const auto options = obj["options"_L1]; options.isObject()) {
                tiebreak->setOptions(options.toObject().toVariantMap());
            }

            m_tiebreaks.push_back(std::move(tiebreak));
        }
    }

    return {};
}

#include "moc_tournament.cpp"
