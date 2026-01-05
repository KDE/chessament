// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QObject>
#include <QString>
#include <QTest>

#include "event.h"
#include "standing.h"
#include "state.h"
#include "tiebreaks/buchholz.h"
#include "tiebreaks/points.h"
#include "tournament.h"

using namespace Qt::Literals::StringLiterals;

class TiebreaksTest : public QObject
{
    Q_OBJECT

private:
    QList<QStringList> readStandings(const QString &fileName);

private Q_SLOTS:

    void testBuchholz_data();
    void testBuchholz();
};

QList<QStringList> TiebreaksTest::readStandings(const QString &fileName)
{
    QList<QStringList> result;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
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

void TiebreaksTest::testBuchholz_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("tournament_1.txt") << QLatin1StringView(DATA_DIR) + "/tournament_1.txt"_L1;
}

void TiebreaksTest::testBuchholz()
{
    QFETCH(QString, fileName);

    const auto expectedStandings = readStandings(fileName + ".aob"_L1);

    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto tournament = event->importTournament(fileName);
    QVERIFY(tournament.has_value());

    std::vector<std::unique_ptr<Tiebreak>> tiebreaks;
    tiebreaks.push_back(std::make_unique<Points>());
    tiebreaks.push_back(std::make_unique<Buchholz>());
    (*tournament)->setTiebreaks(std::move(tiebreaks));

    const auto state = (*tournament)->getState();
    const auto standings = (*tournament)->getStandings(state);

    for (qsizetype i = 0; i < standings.size(); ++i) {
        const auto &standing = standings.at(i);
        const auto &expected = expectedStandings[i];

        const auto result = QStringList{
            QString::number(standing.player()->startingRank()),
            QString::number(i + 1),
            QString::number(standing.values()[0], 'f', 1),
            QString::number(standing.values()[1], 'f', 1),
        };

        QCOMPARE(result.join(','_L1), expected.join(','_L1));
    }
}

QTEST_GUILESS_MAIN(TiebreaksTest)

#include "aobtest.moc"
