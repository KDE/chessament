// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QObject>
#include <QString>
#include <QTest>

#include "ratinglists/htmlreader.h"
#include "ratinglists/ratinglist.h"
#include "ratinglists/ratinglistsmanager.h"

using namespace Qt::StringLiterals;

class RatingListTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testHtmlRatingList_data();
    void testHtmlRatingList();

    void cleanupTestCase();
};

void RatingListTest::testHtmlRatingList_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("ratinglist.html") << u"/ratinglist.html"_s;
    QTest::newRow("ratinglist_2.html") << u"/ratinglist_2.html"_s;
}

void RatingListTest::testHtmlRatingList()
{
    QFETCH(QString, fileName);

    const auto list = std::make_unique<RatingList>(u"Test Rating List"_s);
    auto reader = std::make_unique<HtmlRatingListReader>(list.get());

    QFile file{QLatin1String(DATA_DIR) % fileName};
    QVERIFY(file.open(QFile::ReadOnly));

    QTextStream stream{&file};

    QVERIFY(RatingListsManager::instance().readPlayers(list.get(), &stream, std::move(reader)));

    const auto players = RatingListsManager::searchPlayers(u"K"_s);

    QVERIFY(players);
    QCOMPARE(players->size(), 2);

    const auto &konqi = players->at(0);
    const auto &katie = players->at(1);

    QCOMPARE(konqi.name(), u"The Konqueror, Konqi"_s);
    QCOMPARE(konqi.federation(), QString{});
    QCOMPARE(konqi.gender(), QString{});
    QCOMPARE(konqi.title(), QString{});
    QCOMPARE(konqi.birthDate(), u"1999"_s);
    QCOMPARE(konqi.standardRating(), 3265);
    QCOMPARE(konqi.rapidRating(), 0);
    QCOMPARE(konqi.blitzRating(), 0);
    QCOMPARE(konqi.nationalId(), u"123456789"_s);
    QCOMPARE(konqi.nationalRating(), 3265);

    QCOMPARE(katie.name(), u"Katie"_s);
    QCOMPARE(katie.federation(), QString{});
    QCOMPARE(katie.gender(), QString{});
    QCOMPARE(katie.title(), QString{});
    QCOMPARE(katie.birthDate(), u"2001"_s);
    QCOMPARE(katie.standardRating(), 3182);
    QCOMPARE(katie.rapidRating(), 0);
    QCOMPARE(katie.blitzRating(), 0);
    QCOMPARE(katie.nationalId(), u"123456789"_s);
    QCOMPARE(katie.nationalRating(), 3182);

    RatingListsManager::remove(list->id());
}

void RatingListTest::cleanupTestCase()
{
    QDir().remove(RatingListsManager::databasePath());
}

QTEST_GUILESS_MAIN(RatingListTest)
#include "ratinglisttest.moc"
