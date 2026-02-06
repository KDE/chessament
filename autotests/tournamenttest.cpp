// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>
#include <QString>
#include <QTemporaryFile>
#include <QTest>

#include "event.h"

using namespace Qt::Literals::StringLiterals;

class TournamentTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testNewTournament();
    void testToJson();
    void testTrf();
    void testImportTrf();
    void testLoadTournament();
    void testSortPlayers();
    void testRemovePairings_data();
    void testRemovePairings();
};

void TournamentTest::testNewTournament()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto t = event->createTournament();

    QVERIFY(t.has_value());
    QCOMPARE((*t)->name(), u""_s);
}

void TournamentTest::testToJson()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto t = event->createTournament();
    QVERIFY(t.has_value());

    (*t)->setName(u"Test tournament"_s);

    auto json = (*t)->toJson();

    QCOMPARE(json[u"tournament"_s].toObject()[u"name"_s], u"Test tournament"_s);
}

void TournamentTest::testTrf()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto t = event->createTournament();
    QVERIFY(t.has_value());

    (*t)->setName(u"Test tournament"_s);

    auto trf = (*t)->toTrf();

    QVERIFY(trf.contains(u"012 Test tournament"_s));

    event = std::make_unique<Event>();
    QVERIFY(event->open());

    t = event->createTournament();
    QVERIFY(t.has_value());

    auto ok = (*t)->readTrf(QTextStream(&trf));

    QVERIFY(ok.has_value() && ok);
}

void TournamentTest::testImportTrf()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.txt"_s);
    QVERIFY(tournament.has_value());

    auto t = *tournament;

    QCOMPARE(t->name(), u"Test Tournament"_s);
    QCOMPARE(t->city(), u"Place"_s);
    QCOMPARE(t->federation(), u"ESP"_s);
    QCOMPARE(t->chiefArbiter(), u"Chief Arbiter"_s);
    QCOMPARE(t->deputyChiefArbiter(), u"Arbiter"_s);
    QCOMPARE(t->timeControl(), u"8 min/player + 3 s/move"_s);

    QCOMPARE(t->numberOfPlayers(), 88);
    QCOMPARE(t->numberOfRatedPlayers(), 82);
    QCOMPARE(t->numberOfRounds(), 9);
    QCOMPARE(t->initialColor(), Tournament::InitialColor::White);

    QCOMPARE(t->pairings(1).size(), 44);
    for (int i = 2; i <= 9; ++i) {
        QCOMPARE(t->pairings(i).size(), 46);
    }
}

void TournamentTest::testLoadTournament()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.txt"_s);
    QVERIFY(tournament.has_value());

    QTemporaryFile file;
    QVERIFY(file.open());
    QVERIFY(!file.fileName().isEmpty());

    event->saveAs(file.fileName());

    event = std::make_unique<Event>();
    QVERIFY(event->open(file.fileName()).has_value());

    QCOMPARE(event->numberOfTournaments(), 1);

    auto t = event->tournament(0);

    QCOMPARE(t->name(), u"Test Tournament"_s);
    QCOMPARE(t->city(), u"Place"_s);
    QCOMPARE(t->federation(), u"ESP"_s);
    QCOMPARE(t->chiefArbiter(), u"Chief Arbiter"_s);
    QCOMPARE(t->deputyChiefArbiter(), u"Arbiter"_s);
    QCOMPARE(t->timeControl(), u"8 min/player + 3 s/move"_s);

    QCOMPARE(t->numberOfPlayers(), 88);
    QCOMPARE(t->numberOfRatedPlayers(), 82);
    QCOMPARE(t->numberOfRounds(), 9);
    QCOMPARE(t->initialColor(), Tournament::InitialColor::White);

    QCOMPARE(t->pairings(1).size(), 44);
    for (int i = 2; i <= 9; ++i) {
        QCOMPARE(t->pairings(i).size(), 46);
    }
}

void TournamentTest::testSortPlayers()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_2.trf"_s);
    QVERIFY(tournament.has_value());

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

    QTest::newRow("keepByes = false") << false << QList<int>{44, 46, 46, 46, 0, 0, 0, 0, 0};
    QTest::newRow("keepByes = true") << true << QList<int>{44, 46, 46, 46, 3, 3, 4, 4, 4};
}

void TournamentTest::testRemovePairings()
{
    QFETCH(bool, keepByes);
    QFETCH(QList<int>, pairings);

    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto tournament = event->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.txt"_s);
    QVERIFY(tournament.has_value());

    QVERIFY((*tournament)->removePairings(5, keepByes));

    for (int i = 1; i <= 9; ++i) {
        QCOMPARE((*tournament)->pairings(i).size(), pairings[i - 1]);
    }
}

QTEST_GUILESS_MAIN(TournamentTest)
#include "tournamenttest.moc"
