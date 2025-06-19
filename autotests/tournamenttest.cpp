// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
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
};

void TournamentTest::testNewTournament()
{
    auto e = std::make_unique<Event>();
    auto t = e->createTournament();
    QCOMPARE(t->name(), u""_s);
}

void TournamentTest::testToJson()
{
    auto e = std::make_unique<Event>();
    auto t = e->createTournament();
    t->setName(u"Test tournament"_s);

    auto json = t->toJson();

    QCOMPARE(json[u"tournament"_s].toObject()[u"name"_s], u"Test tournament"_s);
}

void TournamentTest::testTrf()
{
    auto e = std::make_unique<Event>();
    auto t = e->createTournament();
    t->setName(u"Test tournament"_s);

    auto trf = t->toTrf();

    QVERIFY(trf.contains(u"012 Test tournament"_s));

    e = std::make_unique<Event>();
    t = e->createTournament();
    auto ok = t->readTrf(QTextStream(&trf));

    QVERIFY(ok.has_value() && ok);
}

void TournamentTest::testImportTrf()
{
    auto e = std::make_unique<Event>();
    auto tournament = e->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.txt"_s);

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

    QCOMPARE(t->getPairings(1)->size(), 44);
    for (int i = 2; i <= 9; ++i) {
        QCOMPARE(t->getPairings(i)->size(), 46);
    }
}

void TournamentTest::testLoadTournament()
{
    auto e = std::make_unique<Event>();
    auto tournament = e->importTournament(QLatin1String(DATA_DIR) + u"/tournament_1.txt"_s);

    QVERIFY(tournament.has_value());

    QTemporaryFile file;
    QVERIFY(file.open());
    QVERIFY(!file.fileName().isEmpty());

    e->saveAs(file.fileName());

    e = std::make_unique<Event>(file.fileName());
    QCOMPARE(e->numberOfTournaments(), 1);

    auto t = e->getTournament(0);

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

    QCOMPARE(t->getPairings(1)->size(), 44);
    for (int i = 2; i <= 9; ++i) {
        QCOMPARE(t->getPairings(i)->size(), 46);
    }
}

QTEST_GUILESS_MAIN(TournamentTest)
#include "tournamenttest.moc"
