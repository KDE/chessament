// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
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

std::expected<void, QString> Event::create(const QString &fileName)
{
    const QString dbName = fileName.isEmpty() ? ":memory:"_L1 : fileName;

    if (const auto ok = openDatabase(dbName); !ok) {
        return ok;
    }

    setFileName(fileName);

    return {};
}

std::expected<void, QString> Event::open(const QString &fileName)
{
    Q_ASSERT(!fileName.isEmpty());

    setFileName(fileName);

    if (auto ok = openDatabase(fileName); !ok) {
        return ok;
    }
    if (auto ok = loadTournaments(); !ok) {
        return ok;
    }

    return {};
}

Tournament *Event::tournament(uint index)
{
    return m_tournaments.at(index).get();
}

std::expected<Tournament *, QString> Event::createTournament()
{
    auto tournament = std::unique_ptr<Tournament>(new Tournament(this));
    if (const auto ok = tournament->createNewTournament(); !ok) {
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

    if (const auto ok = (*tournament)->loadTrf(fileName); !ok) {
        return std::unexpected(ok.error());
    }

    return m_tournaments.back().get();
}

std::expected<void, QString> Event::saveAs(const QString &fileName)
{
    QSqlQuery query(db());
    query.prepare(u"VACUUM INTO :fileName;"_s);
    query.bindValue(u":fileName"_s, fileName);

    if (!query.exec()) {
        qDebug() << "save copy" << query.lastError();
        return std::unexpected(query.lastError().text());
    }

    return {};
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

QSqlDatabase Event::db()
{
    return QSqlDatabase::database(m_connName);
}

std::expected<void, QString> Event::openDatabase(const QString &dbName)
{
    Q_ASSERT(m_connName.isEmpty());

    m_connName = QUuid::createUuid().toString(QUuid::WithoutBraces);

    auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, m_connName);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "error while opening database";
        return std::unexpected(db.lastError().text());
    }

    // If the file already exists, check that it's a Chessament event
    if (!m_fileName.isEmpty()) {
        QSqlQuery query(u"PRAGMA application_id;"_s, db);

        if (query.lastError().isValid()) {
            qDebug() << "error while quering app id" << query.lastError().text() << query.lastError().nativeErrorCode();

            if (query.lastError().nativeErrorCode() == SQLITE_NOTADB) {
                return std::unexpected(xi18nc("@info", "The file is not a <application>Chessament</application> event."));
            }
            return std::unexpected(query.lastError().text());
        }

        query.next();

        const auto applicationId = query.value(0).toInt();
        if (applicationId != CHESSAMENT_MAGIC_APPLICATION_ID) {
            return std::unexpected(xi18nc("@info", "The file is not a <application>Chessament</application> event."));
        }
    }

    if (auto ok = createTables(); !ok) {
        qDebug() << "error creating tables" << ok.error();
        return ok;
    }

    return {};
}

void Event::closeDatabase()
{
    db().close();
    QSqlDatabase::removeDatabase(m_connName);
}

std::expected<void, QString> Event::createTables()
{
    QSqlQuery query(ENABLE_FOREIGN_KEYS_QUERY, db());

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db());
    query.prepare(TOURNAMENTS_TABLE_SCHEMA);

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db());
    query.prepare(OPTIONS_TABLE_SCHEMA);

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db());
    query.prepare(PLAYERS_TABLE_SCHEMA);

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db());
    query.prepare(ROUNDS_TABLE_SCHEMA);

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    query = QSqlQuery(db());
    query.prepare(PAIRINGS_TABLE_SCHEMA);

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    if (const auto ok = setDbVersion(1); !ok) {
        return ok;
    }

    query = QSqlQuery(db());
    query.prepare(u"PRAGMA application_id = %1;"_s.arg(CHESSAMENT_MAGIC_APPLICATION_ID));

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    return {};
}

std::expected<int, QString> Event::dbVersion()
{
    QSqlQuery query(u"PRAGMA user_version;"_s, db());

    if (query.lastError().isValid()) {
        return std::unexpected(query.lastError().text());
    }

    return query.value(0).toInt();
}

std::expected<void, QString> Event::setDbVersion(int version)
{
    QSqlQuery query(db());
    query.prepare(u"PRAGMA user_version = %1;"_s.arg(version)); // Can't bind in a PRAGMA statement

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    return {};
}

std::expected<void, QString> Event::loadTournaments()
{
    QSqlQuery query(db());
    query.prepare(GET_TOURNAMENTS_QUERY);

    if (!query.exec()) {
        return std::unexpected(query.lastError().text());
    }

    const int idNo = query.record().indexOf("id");

    while (query.next()) {
        const auto id = query.value(idNo).toString();

        auto tournament = std::unique_ptr<Tournament>(new Tournament(this));
        if (const auto ok = tournament->loadTournament(id); !ok) {
            return ok;
        }

        m_tournaments.push_back(std::move(tournament));
    }

    return {};
}

#include "moc_event.cpp"
