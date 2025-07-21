// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QObject>
#include <QString>
#include <QTest>

#include "event.h"
#include "state.h"
#include "tiebreaks/buchholz.h"
#include "tiebreaks/points.h"
#include "tournament.h"

using namespace Qt::Literals::StringLiterals;

class BuchholzTest : public QObject
{
    Q_OBJECT

private:
    QList<QStringList> readStandings(const QString &fileName);

private Q_SLOTS:

    void testBuchholz_data();
    void testBuchholz();
};

QList<QStringList> BuchholzTest::readStandings(const QString &fileName)
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

void BuchholzTest::testBuchholz_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("tournament_1.txt") << QLatin1StringView(DATA_DIR) + "/tournament_1.txt"_L1;
    QTest::newRow("buchholz_1.trf") << QLatin1StringView(DATA_DIR) + "/buchholz_1.trf"_L1;
    QTest::newRow("buchholz_2.trf") << QLatin1StringView(DATA_DIR) + "/buchholz_2.trf"_L1;
    QTest::newRow("buchholz_3.trf") << QLatin1StringView(DATA_DIR) + "/buchholz_3.trf"_L1;
    QTest::newRow("buchholz_4.trf") << QLatin1StringView(DATA_DIR) + "/buchholz_4.trf"_L1;
}

void BuchholzTest::testBuchholz()
{
    QFETCH(QString, fileName);

    const auto expectedStandings = readStandings(fileName + ".standings"_L1);

    auto event = std::make_unique<Event>();
    auto tournament = event->importTournament(fileName);

    QVERIFY(tournament.has_value());

    (*tournament)->setTiebreaks({new Points(), new Buchholz()});

    const auto state = (*tournament)->getState();
    const auto standings = (*tournament)->getStandings(state);

    for (qsizetype i = 0; i < standings.size(); ++i) {
        const auto &standing = standings.at(i);
        const auto player = standing.first;
        const auto &expected = expectedStandings[i];

        const auto result = QStringList{
            QString::number(player->startingRank()),
            QString::number(i + 1),
            QString::number(standing.second[0], 'f', 1),
            QString::number(standing.second[1], 'f', 1),
        };

        QCOMPARE(result.join(','_L1), expected.join(','_L1));
    }
}

QTEST_GUILESS_MAIN(BuchholzTest)
#include "buchholztest.moc"
