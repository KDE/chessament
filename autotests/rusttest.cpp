// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>
#include <QString>
#include <QTemporaryFile>
#include <QTest>

#include "tournament/src/event.cxxqt.h"

using namespace Qt::Literals::StringLiterals;

class RustTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testNewTournament();
};

void RustTest::testNewTournament()
{
    auto event = std::make_unique<Event>();
    QVERIFY(event->open());

    auto t = event->createTournament();

    QVERIFY(t.has_value());
    QCOMPARE((*t)->name(), u""_s);
}

QTEST_GUILESS_MAIN(RustTest)
#include "rusttest.moc"
