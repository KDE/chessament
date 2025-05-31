// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "event.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "db.h"

Event::Event(const QString &fileName)
    : QObject()
{
    openDatabase(fileName);
    loadTournaments();
}

QList<Tournament *> Event::tournaments()
{
    return m_tournaments;
}

QString Event::fileName()
{
    return m_fileName;
}

void Event::setFileName(const QString &fileName)
{
    if (m_fileName == fileName) {
        return;
    }
    m_fileName = fileName;
    Q_EMIT fileNameChanged();
}

Tournament *Event::createTournament()
{
    auto tournament = new Tournament(this);
    m_tournaments.append(tournament);

    return tournament;
}

std::expected<Tournament *, QString> Event::importTournament(const QString &fileName)
{
    auto tournament = new Tournament(this);
    auto error = tournament->loadTrf(fileName);

    if (error.has_value()) {
        return tournament;
    }
    return std::unexpected(error.error());
}

void Event::saveAs(const QString &fileName)
{
    QSqlQuery query(getDB());
    query.prepare(u"VACUUM INTO :fileName;"_s);
    query.bindValue(u":fileName"_s, fileName);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "save copy" << query.lastError();
    }
}

QSqlDatabase Event::getDB()
{
    return QSqlDatabase::database(m_connName);
}

bool Event::openDatabase(const QString &fileName)
{
    QString dbName;

    if (fileName.isEmpty()) {
        m_connName = QUuid::createUuid().toString(QUuid::WithoutBraces);
        dbName = u":memory:"_s;
    } else {
        m_connName = fileName;
        dbName = fileName;
    }
    qDebug() << m_connName;

    auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, m_connName);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "error while opening database";
        return false;
    }

    createTables();
    return true;
}

void Event::createTables()
{
    QSqlQuery query(u"PRAGMA foreign_keys = ON;"_s, getDB());

    query = QSqlQuery(getDB());
    query.prepare(TOURNAMENTS_TABLE_SCHEMA);
    query.exec();

    query = QSqlQuery(getDB());
    query.prepare(OPTIONS_TABLE_SCHEMA);
    query.exec();

    query = QSqlQuery(getDB());
    query.prepare(PLAYERS_TABLE_SCHEMA);
    query.exec();

    query = QSqlQuery(getDB());
    query.prepare(ROUNDS_TABLE_SCHEMA);
    query.exec();

    query = QSqlQuery(getDB());
    query.prepare(PAIRINGS_TABLE_SCHEMA);
    query.exec();

    setDBVersion(1);
}

int Event::getDBVersion()
{
    QSqlQuery query(u"PRAGMA user_version;"_s, getDB());
    return query.value(0).toInt();
}

void Event::setDBVersion(int version)
{
    QSqlQuery query(getDB());
    query.prepare(u"PRAGMA user_version = %1;"_s.arg(version)); // Can't bind in a PRAGMA statement
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "set db version" << query.lastError();
    }
}

void Event::loadTournaments()
{
    QSqlQuery query(getDB());
    query.prepare(GET_TOURNAMENTS_QUERY);
    query.exec();

    int idNo = query.record().indexOf("id");

    while (query.next()) {
        auto id = query.value(idNo).toString();

        auto tournament = new Tournament(this, id);
        m_tournaments << tournament;
    }
}
