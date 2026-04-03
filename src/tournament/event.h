// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

#include "account.h"
#include "sync/syncengine.h"
#include "tournament.h"

/*!
 * \class Event
 * \inmodule tournament
 * \inheaderfile tournament/event.h
 */
class Event : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool syncEnabled READ syncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)
    Q_PROPERTY(SyncEngine::Status syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString syncStatusString READ syncStatusString NOTIFY syncStatusChanged)

public:
    explicit Event() = default;

    ~Event() override;

    /*!
     * \property Event::fileName
     * \brief the file name of the event
     *
     * This property holds the file name of the event.
     */
    QString fileName();

    /*!
     * Returns the number of tournaments in the event.
     */
    size_t numberOfTournaments();

    std::expected<void, QString> open(const QString &fileName = {});

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
    void saveAs(const QString &fileName);

    /*!
     * Deletes the event.
     *
     * Returns true if successful; false otherwise.
     */
    bool remove();

    [[nodiscard]] bool syncEnabled() const;

    SyncEngine *addSyncEngine(Account *account);

    [[nodiscard]] SyncEngine::Status syncStatus() const;

    [[nodiscard]] QString syncStatusString() const;

public Q_SLOTS:
    void setFileName(const QString &fileName);

    void setSyncEnabled(bool enabled);

Q_SIGNALS:
    void fileNameChanged();

    void syncEnabledChanged();

    void syncStatusChanged();

private:
    QSqlDatabase db();
    std::expected<void, QString> openDatabase();
    void closeDatabase();
    std::expected<void, QString> createTables();
    std::expected<int, QString> dbVersion();
    std::expected<void, QString> setDbVersion(int version);
    std::expected<void, QString> loadTournaments();

    QString m_connName;
    QString m_fileName;

    std::vector<std::unique_ptr<Tournament>> m_tournaments;

    bool m_syncEnabled = false;
    std::unique_ptr<SyncEngine> m_syncEngine;

    friend class Tournament;
};
