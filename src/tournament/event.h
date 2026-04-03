// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

#ifdef BUILD_EXPERIMENTAL
#include "sync/account.h"
#include "sync/syncengine.h"
#endif

#include "tournament.h"

/*!
 * \class Event
 * \inmodule libtournament
 * \inheaderfile tournament/event.h
 */
class Event : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

#ifdef BUILD_EXPERIMENTAL
    Q_PROPERTY(bool syncEnabled READ syncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)
    Q_PROPERTY(SyncEngine::Status syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString syncStatusString READ syncStatusString NOTIFY syncStatusChanged)
#endif

public:
    explicit Event() = default;

    ~Event() override;

    /*!
     * \property Event::fileName
     *
     * This property holds the file name of the event.
     */
    QString fileName();

    /*!
     * Returns the number of tournaments in the event.
     */
    size_t numberOfTournaments();

    /*!
     * Creates a new event.
     *
     * \a fileName The location to save the event. If it is empty it will be stored in memory.
     */
    std::expected<void, QString> create(const QString &fileName = {});

    /*!
     * Opens the event stored in \fileName.
     */
    std::expected<void, QString> open(const QString &fileName);

    /*!
     * Returns the tournament with index \a index.
     */
    Tournament *tournament(uint index);

    /*!
     * Returns a new tournament.
     */
    std::expected<Tournament *, QString> createTournament();

    /*!
     * Imports a Tournament Report File (TRF) as a new tournament.
     *
     * \a fileName The location of the file
     */
    std::expected<Tournament *, QString> importTournament(const QString &fileName);

    /*!
     * Saves the event in a new file.
     *
     * \a fileName The file name.
     */
    std::expected<void, QString> saveAs(const QString &fileName);

    /*!
     * Permanently deletes the event.
     *
     * Returns true if successful; false otherwise.
     */
    bool remove();

#ifdef BUILD_EXPERIMENTAL
    [[nodiscard]] bool syncEnabled() const;

    SyncEngine *addSyncEngine(Account *account);

    [[nodiscard]] SyncEngine::Status syncStatus() const;

    [[nodiscard]] QString syncStatusString() const;
#endif

public Q_SLOTS:
    void setFileName(const QString &fileName);

#ifdef BUILD_EXPERIMENTAL
    void setSyncEnabled(bool enabled);
#endif

Q_SIGNALS:
    void fileNameChanged();

#ifdef BUILD_EXPERIMENTAL
    void syncEnabledChanged();

    void syncStatusChanged();
#endif

private:
    QSqlDatabase db();
    std::expected<void, QString> openDatabase(const QString &dbName);
    void closeDatabase();
    std::expected<void, QString> createTables();
    std::expected<int, QString> dbVersion();
    std::expected<void, QString> setDbVersion(int version);
    std::expected<void, QString> loadTournaments();

    QString m_connName;
    QString m_fileName;

    std::vector<std::unique_ptr<Tournament>> m_tournaments;

#ifdef BUILD_EXPERIMENTAL
    bool m_syncEnabled = false;
    std::unique_ptr<SyncEngine> m_syncEngine;
#endif

    friend class Tournament;
};
