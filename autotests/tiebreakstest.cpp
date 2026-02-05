// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QObject>
#include <QString>
#include <QTest>

#include "event.h"
#include "standing.h"
#include "state.h"
#include "tournament.h"

using namespace Qt::Literals::StringLiterals;

class TiebreaksTest : public QObject
{
    Q_OBJECT

private:
    QList<QStringList> readStandings(const QString &fileName);

private Q_SLOTS:

    void testTiebreaks_data();
    void testTiebreaks();
};

QList<QStringList> TiebreaksTest::readStandings(const QString &fileName)
{
    QList<QStringList> result;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Q_ASSERT(false);
        return {};
    }

    QTextStream stream(&file);

    QString line;
    while (!stream.atEnd()) {
        stream.readLineInto(&line);

        if (line.isEmpty() || line.startsWith('#'_L1)) {
            continue;
        }

        const auto values = line.split(','_L1);
        result << values;
    }

    return result;
}

void TiebreaksTest::testTiebreaks_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<int>("precision");
    QTest::addColumn<QString>("tiebreaks");

    QTest::newRow("tournament_1.txt BH") << u"tournament_1.txt"_s << u".bh"_s << 1 << u"pts,bh"_s;
    QTest::newRow("tournament_1.txt WIN") << u"tournament_1.txt"_s << u".win"_s << 1 << u"pts,win"_s;
    QTest::newRow("tournament_1.txt WON") << u"tournament_1.txt"_s << u".won"_s << 1 << u"pts,won"_s;
    QTest::newRow("buchholz_1.trf BH") << u"buchholz_1.trf"_s << u".bh"_s << 1 << u"pts,bh"_s;
    QTest::newRow("buchholz_2.trf BH") << u"buchholz_2.trf"_s << u".bh"_s << 1 << u"pts,bh"_s;
    QTest::newRow("buchholz_3.trf BH") << u"buchholz_3.trf"_s << u".bh"_s << 1 << u"pts,bh"_s;
    QTest::newRow("buchholz_4.trf BH") << u"buchholz_4.trf"_s << u".bh"_s << 1 << u"pts,bh"_s;
    QTest::newRow("tournament_1.txt AOB") << u"tournament_1.txt"_s << u".aob"_s << 2 << u"pts,aob"_s;
}

void TiebreaksTest::testTiebreaks()
{
    QFETCH(QString, fileName);
    QFETCH(QString, expected);
    QFETCH(int, precision);
    QFETCH(QString, tiebreaks);

    const auto expectedStandings = readStandings(QLatin1StringView(DATA_DIR) + "/"_L1 + fileName + expected);

    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto tournament = event->importTournament(QLatin1StringView(DATA_DIR) + "/"_L1 + fileName);
    QVERIFY(tournament.has_value());

    QVERIFY((*tournament)->setTiebreaksFromTrf(tiebreaks));

    const State state = (*tournament)->getState();
    const auto standings = (*tournament)->getStandings(state);

    for (qsizetype i = 0; i < standings.size(); ++i) {
        const auto &standing = standings.at(i);
        const auto &expected = expectedStandings[i];

        const auto result = QStringList{
            QString::number(standing.player()->startingRank()),
            QString::number(standing.rank()),
            QString::number(standing.values()[0], 'f', 1),
            QString::number(standing.values()[1], 'f', precision),
        };

        QCOMPARE(result.join(','_L1), expected.join(','_L1));
    }
}

QTEST_GUILESS_MAIN(TiebreaksTest)

#include "tiebreakstest.moc"
