// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "event.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "db.h"

Event::Event(const QString &fileName)
{
    openDatabase(fileName);
    loadTournaments();
}

Event::~Event()
{
    closeDatabase();
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

size_t Event::numberOfTournaments()
{
    return m_tournaments.size();
}

Tournament *Event::getTournament(uint index)
{
    return m_tournaments.at(index).get();
}

Tournament *Event::createTournament()
{
    auto tournament = std::unique_ptr<Tournament>(new Tournament(this));
    m_tournaments.push_back(std::move(tournament));

    return m_tournaments.back().get();
}

std::expected<Tournament *, QString> Event::importTournament(const QString &fileName)
{
    auto tournament = std::unique_ptr<Tournament>(new Tournament(this));
    auto error = tournament->loadTrf(fileName);

    if (error.has_value()) {
        m_tournaments.push_back(std::move(tournament));

        return m_tournaments.back().get();
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

bool Event::remove()
{
    closeDatabase();

    if (!m_fileName.isEmpty()) {
        QFile db(m_fileName);
        return db.remove();
    }

    return true;
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

void Event::closeDatabase()
{
    getDB().close();
    QSqlDatabase::removeDatabase(m_connName);
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

        auto tournament = std::unique_ptr<Tournament>(new Tournament(this, id));
        m_tournaments.push_back(std::move(tournament));
    }
}
