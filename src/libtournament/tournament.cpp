// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tournament.h"

#include <KLocalizedString>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <algorithm>
#include <iostream>
#include <qassert.h>

#include "db.h"
#include "event.h"
#include "tiebreaks.h"
#include "tournamentstate.h"

Tournament::Tournament(Event *event, const QString &id)
    : m_event(event)
    , m_id(id)
    , m_players(new QList<Player *>())
    , m_rounds(QList<Round *>())
{
    m_tiebreaks = {new Points(), new Buchholz()};

    if (m_id.isEmpty()) {
        createNewTournament();
    } else {
        loadTournament();
    }
}

Tournament::~Tournament()
{
    qDebug() << "delete tournament";
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

int Tournament::numberOfRounds()
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

int Tournament::currentRound()
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

QList<Tiebreak *> Tournament::tiebreaks()
{
    return m_tiebreaks;
}

void Tournament::setTiebreaks(QList<Tiebreak *> tiebreaks)
{
    if (m_tiebreaks == tiebreaks) {
        return;
    }
    m_tiebreaks = tiebreaks;
    Q_EMIT tiebreaksChanged();
}

QList<Player *> *Tournament::players()
{
    return m_players;
}

void Tournament::setPlayers(QList<Player *> *players)
{
    m_players = players;
    Q_EMIT playersChanged();
}

void Tournament::addPlayer(Player *player)
{
    m_players->append(player);

    QSqlQuery query(m_event->getDB());
    query.prepare(ADD_PLAYER_QUERY);
    query.bindValue(u":startingRank"_s, player->startingRank());
    query.bindValue(u":title"_s, Player::titleString(player->title()));
    query.bindValue(u":name"_s, player->name());
    query.bindValue(u":surname"_s, player->surname());
    query.bindValue(u":rating"_s, player->rating());
    query.bindValue(u":nationalRating"_s, player->nationalRating());
    query.bindValue(u":playerId"_s, player->playerId());
    query.bindValue(u":birthDate"_s, player->birthDate());
    query.bindValue(u":federation"_s, player->federation());
    query.bindValue(u":origin"_s, player->origin());
    query.bindValue(u":sex"_s, player->sex());
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "add player" << *player << query.lastError();
    }

    player->setId(query.lastInsertId().toInt());

    Q_EMIT numberOfPlayersChanged();
}

void Tournament::savePlayer(Player *player)
{
    QSqlQuery query(m_event->getDB());
    query.prepare(UPDATE_PLAYER_QUERY);
    query.bindValue(u":id"_s, player->id());
    query.bindValue(u":startingRank"_s, player->startingRank());
    query.bindValue(u":title"_s, Player::titleString(player->title()));
    query.bindValue(u":name"_s, player->name());
    query.bindValue(u":surname"_s, player->surname());
    query.bindValue(u":rating"_s, player->rating());
    query.bindValue(u":nationalRating"_s, player->nationalRating());
    query.bindValue(u":playerId"_s, player->playerId());
    query.bindValue(u":birthDate"_s, player->birthDate());
    query.bindValue(u":federation"_s, player->federation());
    query.bindValue(u":origin"_s, player->origin());
    query.bindValue(u":sex"_s, player->sex());
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "save player" << *player << query.lastError();
    }
}

QMap<uint, Player *> Tournament::getPlayersByStartingRank()
{
    QMap<uint, Player *> players;

    for (const auto &player : std::as_const(*m_players)) {
        players[player->startingRank()] = player;
    }

    return players;
}

QMap<uint, Player *> Tournament::getPlayersById()
{
    QMap<uint, Player *> players;

    for (const auto &player : std::as_const(*m_players)) {
        players[player->id()] = player;
    }

    return players;
}

QHash<Player *, QList<Pairing *>> Tournament::getPairingsByPlayer(int maxRound)
{
    QHash<Player *, QList<Pairing *>> pairings;

    auto r = maxRound < 0 ? m_rounds.size() : maxRound;
    for (int i = 0; i < r; i++) {
        if (i >= m_rounds.size()) {
            break;
        }
        const auto round = m_rounds.at(i);
        for (const auto &pairing : round->pairings()) {
            pairings[pairing->whitePlayer()] << pairing;
            if (pairing->blackPlayer() != nullptr) {
                pairings[pairing->blackPlayer()] << pairing;
            }
        }
    }

    return pairings;
}

QList<PlayerTiebreaks> Tournament::getStandings(int round)
{
    TournamentState state(this, round);
    QList<PlayerTiebreaks> standings;

    // Sort by tiebreaks
    auto sortStandings = [&standings]() {
        std::sort(standings.begin(), standings.end(), [](PlayerTiebreaks p1, PlayerTiebreaks p2) {
            for (int i = 0; i < p1.second.size(); i++) {
                if (p1.second.at(i) == p2.second.at(i)) {
                    continue;
                }
                return p1.second.at(i) > p2.second.at(i);
            }
            return p1.first->startingRank() < p2.first->startingRank();
        });
    };

    for (auto player = m_players->cbegin(), end = m_players->cend(); player != end; player++) {
        standings << std::make_pair(*player, Tiebreaks{});
    }

    // Calculate tiebreaks
    QList<Player *> players;
    for (const auto &tiebreak : std::as_const(m_tiebreaks)) {
        int i = 0;
        players.clear();
        while (i < m_players->size()) {
            if (players.isEmpty()) {
                players << standings.at(i).first;
                i++;
                continue;
            }
            if (standings.at(i - 1).second == standings.at(i).second) {
                players << standings.at(i).first;
                i++;
            } else {
                for (int j = 0; j < players.size(); j++) {
                    /*for (const auto &p : players) {
                        qDebug() << p->name();
                    }
                    qDebug() << "Player" << j << "is" << players.at(j)->name();
                    qDebug() << "SPlayer" << j << "is" << standings.at(i - players.size() + j).first->name();
                    qDebug() << "--";*/
                    standings[i - players.size() + j].second << tiebreak->calculate(this, state, players, players.at(j));
                }
                players.clear();
                players << standings.at(i).first;
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
            standings[i - players.size() + j].second << tiebreak->calculate(this, state, players, players.at(j));
        }
        sortStandings();
    }

    // Print standings for debugging
    for (int i = 0; i < standings.size(); i++) {
        const auto s = standings.at(i);
        QStringList l;
        for (const auto x : s.second) {
            l << QString::number(x);
        }
        QString t = u"{\"id\": "_s + QString::number(s.first->startingRank()) + u", \"tiebreaks\": ["_s + l.join(u", "_s) + u"]}"_s;
        // qDebug() << i + 1 << s.first->name() << t;
        std::cout << t.toStdString() << std::endl;
    }

    return standings;
}

QList<Round *> Tournament::rounds() const
{
    return m_rounds;
}

void Tournament::setRounds(QList<Round *> rounds)
{
    if (m_rounds == rounds) {
        return;
    }
    m_rounds = rounds;
    Q_EMIT roundsChanged();
}

void Tournament::setInitialColor(Tournament::InitialColor color)
{
    if (m_initialColor == color) {
        return;
    }
    m_initialColor = color;
    setOption(u"initial_color"_s, std::to_underlying(color));
}

void Tournament::addPairing(int round, Pairing *pairing)
{
    while (m_rounds.size() < round) {
        QSqlQuery query(m_event->getDB());
        query.prepare(ADD_ROUND_QUERY);
        query.bindValue(u":number"_s, round);
        query.bindValue(u":tournament"_s, m_id);
        query.exec();

        if (query.lastError().isValid()) {
            qDebug() << "add pairing : round" << query.lastError();
        }

        auto round = new Round();
        round->setId(query.lastInsertId().toInt());
        m_rounds << round;
    }

    m_rounds.at(round - 1)->addPairing(pairing);

    auto roundId = m_rounds.at(round - 1)->id();

    QSqlQuery query(m_event->getDB());
    query.prepare(ADD_PAIRING_QUERY);
    query.bindValue(u":board"_s, pairing->board());
    query.bindValue(u":whitePlayer"_s, pairing->whitePlayer()->id());
    if (pairing->blackPlayer() != nullptr) {
        query.bindValue(u":blackPlayer"_s, pairing->blackPlayer()->id());
    } else {
        query.bindValue(u":blackPlayer"_s, QVariant(QMetaType::fromType<int>()));
    }
    query.bindValue(u":whiteResult"_s, std::to_underlying(pairing->whiteResult()));
    query.bindValue(u":blackResult"_s, std::to_underlying(pairing->blackResult()));
    query.bindValue(u":round"_s, roundId);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "add pairing : pairing" << query.lastError();
    }

    pairing->setId(query.lastInsertId().toInt());
}

void Tournament::savePairing(Pairing *pairing)
{
    QSqlQuery query(m_event->getDB());
    query.prepare(UPDATE_PAIRING_QUERY);
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
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "save pairing" << query.lastError();
    }
}

void Tournament::setResult(Pairing *pairing, Pairing::Result result)
{
    pairing->setResult(result);
    savePairing(pairing);
}

QList<Pairing *> Tournament::getPairings(int round) const
{
    Q_ASSERT(round > 0);
    if (round <= m_rounds.size()) {
        return m_rounds.at(round - 1)->pairings();
    }
    return {};
}

int Tournament::numberOfPlayers()
{
    return m_players->size();
}

int Tournament::numberOfRatedPlayers()
{
    return std::count_if(m_players->cbegin(), m_players->cend(), [](Player *p) {
        return p->rating() > 0;
    });
}

void Tournament::sortPairings()
{
    for (int i = 0; i < m_rounds.size(); i++) {
        auto pairings = m_rounds.at(i)->pairings();

        auto state = getState(i);

        std::sort(pairings.begin(), pairings.end(), [i, &state](Pairing *a, Pairing *b) -> bool {
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
                } else {
                    return std::to_underlying(a->whiteResult()) > std::to_underlying(b->whiteResult());
                }
            } else if (aRank == 0) {
                return false;
            } else if (bRank == 0) {
                return true;
            }

            double aScore;
            double aTotal;
            if (a->whitePlayer()->startingRank() < a->blackPlayer()->startingRank()) {
                aScore = state.getPoints(a->whitePlayer());
                aTotal = aScore + state.getPoints(a->blackPlayer());
            } else {
                aScore = state.getPoints(a->blackPlayer());
                aTotal = aScore + state.getPoints(a->whitePlayer());
            }
            double bScore;
            double bTotal;
            if (b->whitePlayer()->startingRank() < b->blackPlayer()->startingRank()) {
                bScore = state.getPoints(b->whitePlayer());
                bTotal = bScore + state.getPoints(b->blackPlayer());
            } else {
                bScore = state.getPoints(b->blackPlayer());
                bTotal = bScore + state.getPoints(b->whitePlayer());
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

        for (int i = 0; i < pairings.size(); i++) {
            pairings.at(i)->setBoard(i + 1);

            savePairing(pairings.at(i));
        }

        m_rounds.at(i)->setPairings(pairings);
    }
}

bool Tournament::isRoundFinished(int round)
{
    qDebug() << "is round finished" << round;

    if (round <= 0) {
        return true;
    }

    QList<Pairing *> pairings;
    if (round <= m_rounds.size()) {
        pairings = m_rounds.value(round - 1)->pairings();
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
    Q_ASSERT(round <= m_rounds.size());

    auto pairings = m_rounds.value(round - 1)->pairings();

    QSet<Player *> players;

    for (const auto &pairing : pairings) {
        players.insert(pairing->whitePlayer());
        if (pairing->blackPlayer() != nullptr) {
            players.insert(pairing->blackPlayer());
        }
    }

    return m_players->size() == players.size();
}

QCoro::Task<std::expected<QList<Pairing *>, QString>> Tournament::calculatePairings(int round)
{
    auto engine = new PairingEngine();
    const auto pairings = co_await engine->pair(round, this);

    if (!pairings.has_value()) {
        co_return std::unexpected(pairings.error());
    }

    co_return pairings;
}

QCoro::Task<std::expected<bool, QString>> Tournament::pairNextRound()
{
    const auto pairings = co_await calculatePairings(m_currentRound + 1);

    if (!pairings.has_value()) {
        co_return std::unexpected(i18n("An error ocurred while pairing the round: %1", pairings.error()));
    }

    for (const auto &pairing : *pairings) {
        addPairing(m_currentRound + 1, pairing);
    }

    sortPairings();

    setCurrentRound(m_currentRound + 1);

    co_return true;
}

void Tournament::removePairings(int round, bool keepByes)
{
    for (int i = round; i <= m_numberOfRounds; i++) {
        QSqlQuery query(m_event->getDB());
        if (keepByes) {
            query.prepare(DELETE_PAIRINGS_NO_BYES_QUERY);
        } else {
            query.prepare(DELETE_PAIRINGS_QUERY);
        }
        query.bindValue(u":round"_s, m_rounds.at(i - 1)->id());
        query.exec();

        if (query.lastError().isValid()) {
            qDebug() << "remove pairings" << query.lastError();
        }

        m_rounds.at(i - 1)->removePairings([keepByes](Pairing *pairing) {
            return !keepByes || !Pairing::isRequestedBye(pairing->whiteResult());
        });
    }

    setCurrentRound(round - 1);
}

Tournament::InitialColor Tournament::initialColor()
{
    return m_initialColor;
}

TournamentState Tournament::getState(int maxRound)
{
    return TournamentState{this, maxRound};
}

QVariant Tournament::getOption(const QString &name)
{
    QSqlQuery query(m_event->getDB());
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

void Tournament::setOption(const QString &name, QVariant value)
{
    QSqlQuery query(m_event->getDB());
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
    for (const auto &player : std::as_const(*m_players)) {
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
        m_players->clear();
        m_players->reserve(players.size());
        for (const auto &player : std::as_const(players)) {
            *m_players << Player::fromJson(player.toObject());
        }
    }
}

QString Tournament::toTrf(TrfOptions options, int maxRound)
{
    QString result;
    QTextStream stream(&result);

    auto state = getState();
    auto standings = getStandings(-1);

    const auto space = u" "_s;
    const auto newLine = QLatin1Char('\n');

    stream << reportFieldString(ReportField::TournamentName) << space << m_name << newLine;
    stream << reportFieldString(ReportField::City) << space << m_city << newLine;
    stream << reportFieldString(ReportField::Federation) << space << m_federation << newLine;
    stream << reportFieldString(ReportField::NumberOfPlayers) << space << numberOfPlayers() << newLine;
    stream << reportFieldString(ReportField::NumberOfRatedPlayers) << space << numberOfRatedPlayers() << newLine;
    stream << reportFieldString(ReportField::ChiefArbiter) << space << m_chiefArbiter << newLine;
    stream << reportFieldString(ReportField::TimeControl) << space << m_timeControl << newLine;

    if (options & TrfOption::NumberOfRounds) {
        stream << u"XXR "_s + QString::number(m_numberOfRounds) << newLine;
    }

    if (options & TrfOption::InitialColorWhite) {
        stream << u"XXC white1\n"_s;
    } else if (options & TrfOption::InitialColorBlack) {
        stream << u"XXC black1\n"_s;
    }

    const auto r = maxRound < 0 ? m_numberOfRounds : maxRound;

    for (const auto player : std::as_const(*m_players)) {
        const auto standing = std::find_if(standings.constBegin(), standings.constEnd(), [&player](PlayerTiebreaks s) {
            return s.first == player;
        });
        const auto rank = std::distance(standings.constBegin(), standing) + 1;
        const auto result = player->toTrf(state.getPoints(player), rank);

        stream << result.c_str();

        auto pairings = state.getPairings(player);
        for (int i = 0; i < r; i++) {
            if (i < pairings.size()) {
                stream << pairings.value(i)->toTrf(player);
            } else {
                stream << u"          "_s;
            }
        }

        stream << newLine;
    }

    return result;
}

std::expected<bool, QString> Tournament::loadTrf(const QString &filename)
{
    QFile file(filename);

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

bool Tournament::createNewTournament()
{
    const auto newId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QSqlQuery query(m_event->getDB());
    query.prepare(ADD_TOURNAMENT_QUERY);
    query.bindValue(u":id"_s, newId);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "create tournament" << query.lastError();
        return false;
    }

    setId(newId);
    return true;
}

bool Tournament::loadTournament()
{
    loadOptions();
    loadPlayers();
    loadRounds();
    loadPairings();

    return true;
}

void Tournament::loadOptions()
{
    setName(getOption(u"name"_s).toString());
    setCity(getOption(u"city"_s).toString());
    setFederation(getOption(u"federation"_s).toString());
    setChiefArbiter(getOption(u"chief_arbiter"_s).toString());
    setDeputyChiefArbiter(getOption(u"deputy_chief_arbiter"_s).toString());
    setTimeControl(getOption(u"time_control"_s).toString());
    setNumberOfRounds(getOption(u"number_of_rounds"_s).toInt());
    setCurrentRound(getOption(u"current_round"_s).toInt());
    setInitialColor(Tournament::InitialColor(getOption(u"initial_color"_s).toInt()));
}

void Tournament::loadPlayers()
{
    m_players->clear();

    QSqlQuery query(m_event->getDB());
    query.prepare(GET_PLAYERS_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    int idNo = query.record().indexOf("id");
    int stRankNo = query.record().indexOf("startingRank");
    int titleNo = query.record().indexOf("title");
    int nameNo = query.record().indexOf("name");
    int surnameNo = query.record().indexOf("surname");
    int ratingNo = query.record().indexOf("rating");
    int nationalRatingNo = query.record().indexOf("nationalRating");
    int playerIdNo = query.record().indexOf("playerId");
    int birthDateNo = query.record().indexOf("birthDate");
    int federationNo = query.record().indexOf("federation");
    int originNo = query.record().indexOf("origin");
    int sexNo = query.record().indexOf("sex");

    while (query.next()) {
        auto player = new Player(query.value(stRankNo).toInt(),
                                 Player::titleForString(query.value(titleNo).toString()),
                                 query.value(nameNo).toString(),
                                 query.value(surnameNo).toString(),
                                 query.value(ratingNo).toInt(),
                                 query.value(nationalRatingNo).toInt(),
                                 query.value(playerIdNo).toString(),
                                 query.value(birthDateNo).toString(),
                                 query.value(federationNo).toString(),
                                 query.value(originNo).toString(),
                                 query.value(sexNo).toString());
        player->setId(query.value(idNo).toInt());
        m_players->append(player);
    }
}

void Tournament::loadRounds()
{
    QSqlQuery query(m_event->getDB());
    query.prepare(GET_ROUNDS_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    int idNo = query.record().indexOf("id");
    int numberNo = query.record().indexOf("number");

    while (query.next()) {
        auto round = new Round();
        round->setId(query.value(idNo).toInt());
        round->setNumber(query.value(numberNo).toInt());
        m_rounds << round;
    }

    std::sort(m_rounds.begin(), m_rounds.end(), [](Round *a, Round *b) {
        return a->number() < b->number();
    });
}

void Tournament::loadPairings()
{
    auto players = getPlayersById();

    QSqlQuery query(m_event->getDB());
    query.prepare(GET_PAIRINGS_QUERY);
    query.bindValue(u":tournament"_s, m_id);
    query.exec();

    int idNo = query.record().indexOf("id");
    int boardNo = query.record().indexOf("board");
    int whitePlayerNo = query.record().indexOf("whitePlayer");
    int blackPlayerNo = query.record().indexOf("blackPlayer");
    int whiteResultNo = query.record().indexOf("whiteResult");
    int blackResultNo = query.record().indexOf("blackResult");
    int roundNo = query.record().indexOf("round");

    while (query.next()) {
        auto round = query.value(roundNo).toInt();
        auto pairing = new Pairing(query.value(boardNo).toInt(),
                                   players.value(query.value(whitePlayerNo).toInt()),
                                   players.value(query.value(blackPlayerNo).toInt()),
                                   Pairing::PartialResult(query.value(whiteResultNo).toInt()),
                                   Pairing::PartialResult(query.value(blackResultNo).toInt()));
        pairing->setId(query.value(idNo).toInt());
        m_rounds.at(round - 1)->addPairing(pairing);
    }
}
