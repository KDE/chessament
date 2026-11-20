// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include <QObject>
#include <QTest>

#include "elo.h"
#include "pairing.h"

using namespace Qt::Literals::StringLiterals;

class EloTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testEloChange_data();
    void testEloChange();
};

void EloTest::testEloChange_data()
{
    QTest::addColumn<int>("playerRating");
    QTest::addColumn<int>("opponentRating");
    QTest::addColumn<int>("k");
    QTest::addColumn<Pairing::PartialResult>("result");

    QTest::addColumn<double>("change");

    QTest::newRow("Test 1") << 1500 << 1500 << 40 << Pairing::PartialResult::Win << 20.;
    QTest::newRow("Test 2") << 1500 << 1500 << 20 << Pairing::PartialResult::Win << 10.;
    QTest::newRow("Test 3") << 1500 << 1500 << 20 << Pairing::PartialResult::Draw << 0.;
    QTest::newRow("Test 4") << 1500 << 1500 << 20 << Pairing::PartialResult::Lost << -10.;

    QTest::newRow("Test 10") << 1500 << 1900 << 20 << Pairing::PartialResult::Win << 18.4;
    QTest::newRow("Test 11") << 1500 << 1900 << 20 << Pairing::PartialResult::Draw << 8.4;
    QTest::newRow("Test 12") << 1500 << 1900 << 20 << Pairing::PartialResult::Lost << -1.6;

    QTest::newRow("Test 20") << 1500 << 2200 << 20 << Pairing::PartialResult::Win << 18.4;
    QTest::newRow("Test 21") << 1500 << 2200 << 20 << Pairing::PartialResult::Draw << 8.4;
    QTest::newRow("Test 22") << 1500 << 2200 << 20 << Pairing::PartialResult::Lost << -1.6;

    QTest::newRow("Test 30") << 2200 << 1500 << 20 << Pairing::PartialResult::Win << 1.6;
    QTest::newRow("Test 31") << 2200 << 1500 << 20 << Pairing::PartialResult::Draw << -8.4;
    QTest::newRow("Test 32") << 2200 << 1500 << 20 << Pairing::PartialResult::Lost << -18.4;

    QTest::newRow("Test 40") << 2650 << 1500 << 20 << Pairing::PartialResult::Win << 0.;
    QTest::newRow("Test 41") << 2650 << 1500 << 20 << Pairing::PartialResult::Draw << -10.;
    QTest::newRow("Test 42") << 2650 << 1500 << 20 << Pairing::PartialResult::Lost << -20.;

    QTest::newRow("Test 50") << 2650 << 2500 << 20 << Pairing::PartialResult::Win << 6.;
    QTest::newRow("Test 51") << 2650 << 2500 << 20 << Pairing::PartialResult::Draw << -4.;
    QTest::newRow("Test 52") << 2650 << 2500 << 20 << Pairing::PartialResult::Lost << -14.;
}

void EloTest::testEloChange()
{
    QFETCH(int, playerRating);
    QFETCH(int, opponentRating);
    QFETCH(int, k);
    QFETCH(Pairing::PartialResult, result);

    QFETCH(double, change);

    const auto elo = Elo{playerRating, k};

    const auto variation = elo.change(opponentRating, result);

    QCOMPARE(variation.change(), change);
}

QTEST_GUILESS_MAIN(EloTest)
#include "elotest.moc"
