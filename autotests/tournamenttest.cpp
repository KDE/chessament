// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>
#include <QString>
#include <QTemporaryFile>
#include <QTest>

#include "event.h"
#include "timecontrol.h"

using namespace Qt::Literals::StringLiterals;

class TournamentTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testNewTournament();
    void testCreateTournament();
    void testToJson();
    void testExportTrf();
    void testImportTrf();
    void testLoadTournament();
    void testSortPlayers();
    void testRemovePairings_data();
    void testRemovePairings();
    void testTimeControl_data();
    void testTimeControl();
    void testArbiters();
};

void TournamentTest::testNewTournament()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->createTournament();
    QVERIFY(tournament);

    QCOMPARE((*tournament)->name(), u""_s);
    QCOMPARE((*tournament)->currentRound(), 0);
}

void TournamentTest::testCreateTournament()
{
    QTemporaryFile file;
    QVERIFY(file.open());

    auto event = std::make_unique<Event>();
    QVERIFY(event->create(file.fileName()));

    auto tournament = event->createTournament();

    QVERIFY(tournament);
    QCOMPARE((*tournament)->name(), u""_s);
    QCOMPARE((*tournament)->currentRound(), 0);
}

void TournamentTest::testToJson()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->createTournament();
    QVERIFY(tournament);

    (*tournament)->setName(u"Test tournament"_s);

    auto json = (*tournament)->toJson();

    QCOMPARE(json[u"name"_s], u"Test tournament"_s);
}

void TournamentTest::testExportTrf()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.trf"_s);
    QVERIFY(tournament);

    auto trf = (*tournament)->toTrf();

    QVERIFY(trf.contains(u"012 Test Tournament"_s));

    event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto newTournament = event->createTournament();
    QVERIFY(newTournament.has_value());

    auto ok = (*newTournament)->readTrf(QTextStream(&trf));

    QVERIFY(ok);
}

void TournamentTest::testImportTrf()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.trf"_s);
    QVERIFY(tournament.has_value());

    auto t = *tournament;

    QCOMPARE(t->name(), u"Test Tournament"_s);
    QCOMPARE(t->city(), u"Place"_s);
    QCOMPARE(t->federation(), u"ESP"_s);
    // QCOMPARE(t->timeControl(), u"8 min/player + 3 s/move"_s);

    QCOMPARE(t->numberOfPlayers(), 100);
    QCOMPARE(t->numberOfRatedPlayers(), 100);
    QCOMPARE(t->numberOfRounds(), 9);
    QCOMPARE(t->currentRound(), 9);
    QCOMPARE(t->initialColor(), Tournament::InitialColor::White);

    QCOMPARE(t->pairings(1).size(), 52);
    QCOMPARE(t->pairings(2).size(), 54);
    QCOMPARE(t->pairings(3).size(), 52);
    QCOMPARE(t->pairings(4).size(), 51);
    QCOMPARE(t->pairings(5).size(), 51);
    QCOMPARE(t->pairings(6).size(), 50);
    QCOMPARE(t->pairings(7).size(), 51);
    QCOMPARE(t->pairings(8).size(), 51);
    QCOMPARE(t->pairings(9).size(), 51);
}

void TournamentTest::testLoadTournament()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.trf"_s);
    QVERIFY(tournament);

    QTemporaryFile file;
    QVERIFY(file.open());
    QVERIFY(!file.fileName().isEmpty());

    QVERIFY(event->saveAs(file.fileName()));

    event = std::make_unique<Event>();
    QVERIFY(event->open(file.fileName()).has_value());

    QCOMPARE(event->numberOfTournaments(), 1);

    auto t = event->tournament(0);

    QCOMPARE(t->name(), u"Test Tournament"_s);
    QCOMPARE(t->city(), u"Place"_s);
    QCOMPARE(t->federation(), u"ESP"_s);
    // QCOMPARE(t->timeControl(), u"8 min/player + 3 s/move"_s);

    QCOMPARE(t->numberOfPlayers(), 100);
    QCOMPARE(t->numberOfRatedPlayers(), 100);
    QCOMPARE(t->numberOfRounds(), 9);
    QCOMPARE(t->currentRound(), 9);
    QCOMPARE(t->initialColor(), Tournament::InitialColor::White);

    QCOMPARE(t->pairings(1).size(), 52);
    QCOMPARE(t->pairings(2).size(), 54);
    QCOMPARE(t->pairings(3).size(), 52);
    QCOMPARE(t->pairings(4).size(), 51);
    QCOMPARE(t->pairings(5).size(), 51);
    QCOMPARE(t->pairings(6).size(), 50);
    QCOMPARE(t->pairings(7).size(), 51);
    QCOMPARE(t->pairings(8).size(), 51);
    QCOMPARE(t->pairings(9).size(), 51);
}

void TournamentTest::testSortPlayers()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_2.trf"_s);
    QVERIFY(tournament);

    (*tournament)->sortPlayers();

    const auto players = (*tournament)->players();

    QCOMPARE(players[0]->name(), "Player 3"_L1);
    QCOMPARE(players[1]->name(), "Player 2"_L1);
    QCOMPARE(players[2]->name(), "Player 1"_L1);
    QCOMPARE(players[3]->name(), "Player 4"_L1);
    QCOMPARE(players[4]->name(), "Player 5"_L1);
    QCOMPARE(players[5]->name(), "Player 6"_L1);
    QCOMPARE(players[6]->name(), "Player A"_L1);
    QCOMPARE(players[7]->name(), "Player B"_L1);
}

void TournamentTest::testRemovePairings_data()
{
    QTest::addColumn<bool>("keepByes");
    QTest::addColumn<QList<int>>("pairings");

    QTest::newRow("keepByes = false") << false << QList<int>{52, 54, 52, 51, 0, 0, 0, 0, 0};
    QTest::newRow("keepByes = true") << true << QList<int>{52, 54, 52, 51, 1, 0, 1, 2, 2};
}

void TournamentTest::testRemovePairings()
{
    QFETCH(bool, keepByes);
    QFETCH(QList<int>, pairings);

    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.trf"_s);
    QVERIFY(tournament);

    QCOMPARE((*tournament)->currentRound(), 9);

    QVERIFY((*tournament)->removePairings(5, keepByes));

    QCOMPARE((*tournament)->currentRound(), 4);

    for (int i = 1; i <= 9; ++i) {
        QCOMPARE((*tournament)->pairings(i).size(), pairings[i - 1]);
    }
}

void TournamentTest::testTimeControl_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<TimeControl>("timeControl");

    QTest::newRow("600") << u"600"_s << TimeControl{{TimeControlPeriod{std::nullopt, 600, 0}}};
    QTest::newRow("600+10") << u"600+10"_s << TimeControl{{TimeControlPeriod{std::nullopt, 600, 10}}};
    QTest::newRow("40/600+10") << u"40/600+10"_s << TimeControl{{TimeControlPeriod{40, 600, 10}}};
    QTest::newRow("40/600+10:120+10") << u"40/600+10:120+10"_s
                                      << TimeControl{{
                                             TimeControlPeriod{40, 600, 10},
                                             TimeControlPeriod{std::nullopt, 120, 10},
                                         }};
}

void TournamentTest::testTimeControl()
{
    QFETCH(QString, value);
    QFETCH(TimeControl, timeControl);

    QVERIFY(timeControl == TimeControl::fromTrf(value));
}

void TournamentTest::testArbiters()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->create());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/arbiters.trf"_s);
    QVERIFY(tournament);

    const auto arbiters = (*tournament)->arbiters();

    QCOMPARE(arbiters.size(), 5);

    auto arbiter = arbiters.at(0);
    QCOMPARE(arbiter->role(), Arbiter::Role::Chief);
    QCOMPARE(arbiter->title(), "IA"_L1);
    QCOMPARE(arbiter->name(), "Chief Arbiter"_L1);
    QCOMPARE(arbiter->arbiterId(), "123456"_L1);

    arbiter = arbiters.at(1);
    QCOMPARE(arbiter->role(), Arbiter::Role::Deputy);
    QCOMPARE(arbiter->title(), "IA"_L1);
    QCOMPARE(arbiter->name(), "Deputy Chief Arbiter"_L1);
    QCOMPARE(arbiter->arbiterId(), "987654"_L1);

    arbiter = arbiters.at(2);
    QCOMPARE(arbiter->role(), Arbiter::Role::Deputy);
    QCOMPARE(arbiter->title(), "FA"_L1);
    QCOMPARE(arbiter->name(), "Arbiter"_L1);
    QCOMPARE(arbiter->arbiterId(), QString{});

    arbiter = arbiters.at(3);
    QCOMPARE(arbiter->role(), Arbiter::Role::Deputy);
    QCOMPARE(arbiter->title(), QString{});
    QCOMPARE(arbiter->name(), "Arbiter"_L1);
    QCOMPARE(arbiter->arbiterId(), QString{});

    arbiter = arbiters.at(4);
    QCOMPARE(arbiter->role(), Arbiter::Role::Deputy);
    QCOMPARE(arbiter->title(), QString{});
    QCOMPARE(arbiter->name(), "Arbiter"_L1);
    QCOMPARE(arbiter->arbiterId(), "123456"_L1);
}

QTEST_GUILESS_MAIN(TournamentTest)
#include "tournamenttest.moc"
