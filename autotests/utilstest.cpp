// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QObject>
#include <QString>
#include <QTest>

#include "utils.h"

using namespace Qt::Literals::StringLiterals;

class UtilsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testNormalization_data();
    void testNormalization();
};

void UtilsTest::testNormalization_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QTest::newRow("example1") << u""_s << u""_s;
    QTest::newRow("example2") << u"Hello world"_s << u"Hello world"_s;
    QTest::newRow("example3") << u"Hello world!"_s << u"Hello world"_s;
    QTest::newRow("example4") << u"Héllo world"_s << u"Hello world"_s;
    QTest::newRow("example5") << u"Hello, world"_s << u"Hello, world"_s;
    QTest::newRow("example6") << u"Hello world."_s << u"Hello world."_s;
    QTest::newRow("example7") << u"Hello-world"_s << u"Hello-world"_s;
    QTest::newRow("example8") << u"(Hello world)"_s << u"(Hello world)"_s;
    QTest::newRow("example9") << u"ç ñ ö"_s << u"c n o"_s;
}

void UtilsTest::testNormalization()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    const auto result = Utils::normalize(input);
    QCOMPARE(result, expected);
}

QTEST_GUILESS_MAIN(UtilsTest)
#include "utilstest.moc"
