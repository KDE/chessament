// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "event.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "db.h"

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

std::expected<void, QString> Event::open(const QString &fileName)
{
    setFileName(fileName);

    if (auto ok = openDatabase(); !ok) {
        return ok;
    }
    if (auto ok = loadTournaments(); !ok) {
        return ok;
    }

    return {};
}

Tournament *Event::getTournament(uint index)
{
    return m_tournaments.at(index).get();
}

std::expected<Tournament *, QString> Event::createTournament()
{
    auto tournament = std::unique_ptr<Tournament>(new Tournament(this));
    if (auto ok = tournament->createNewTournament(); !ok) {
        return std::unexpected(ok.error());
    }

    m_tournaments.push_back(std::move(tournament));

    return m_tournaments.back().get();
}

std::expected<Tournament *, QString> Event::importTournament(const QString &fileName)
{
    auto tournament = createTournament();
    if (!tournament) {
        return tournament;
    }

    auto ok = (*tournament)->loadTrf(fileName);
    if (!ok) {
        return std::unexpected(ok.error());
    }

    return m_tournaments.back().get();
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

std::expected<void, QString> Event::openDatabase()
{
    Q_ASSERT(m_connName.isEmpty());

    m_connName = QUuid::createUuid().toString(QUuid::WithoutBraces);

    qDebug() << m_fileName;
    QString dbName = m_fileName.isEmpty() ? ":memory:"_L1 : m_fileName;

    auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, m_connName);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "error while opening database";
        return std::unexpected(db.lastError().text());
    }

    if (auto ok = createTables(); !ok) {
        qDebug() << "error creating tables" << ok.error();
        return ok;
    }

    return {};
}

void Event::closeDatabase()
{
    getDB().close();
    QSqlDatabase::removeDatabase(m_connName);
}

std::expected<void, QString> Event::createTables()
{
    QSqlQuery query(u"PRAGMA foreign_keys = ON;"_s, getDB());

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(getDB());
    query.prepare(TOURNAMENTS_TABLE_SCHEMA);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(getDB());
    query.prepare(OPTIONS_TABLE_SCHEMA);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(getDB());
    query.prepare(PLAYERS_TABLE_SCHEMA);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(getDB());
    query.prepare(ROUNDS_TABLE_SCHEMA);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(getDB());
    query.prepare(PAIRINGS_TABLE_SCHEMA);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    if (auto ok = setDBVersion(1); !ok) {
        return ok;
    }

    return {};
}

std::expected<int, QString> Event::getDBVersion()
{
    QSqlQuery query(u"PRAGMA user_version;"_s, getDB());

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    return query.value(0).toInt();
}

std::expected<void, QString> Event::setDBVersion(int version)
{
    QSqlQuery query(getDB());
    query.prepare(u"PRAGMA user_version = %1;"_s.arg(version)); // Can't bind in a PRAGMA statement
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    return {};
}

std::expected<void, QString> Event::loadTournaments()
{
    QSqlQuery query(getDB());
    query.prepare(GET_TOURNAMENTS_QUERY);
    query.exec();

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    int idNo = query.record().indexOf("id");

    while (query.next()) {
        auto id = query.value(idNo).toString();

        auto tournament = std::unique_ptr<Tournament>(new Tournament(this));
        if (auto ok = tournament->loadTournament(id); !ok) {
            return ok;
        }

        m_tournaments.push_back(std::move(tournament));
    }

    return {};
}

#include "moc_event.cpp"
