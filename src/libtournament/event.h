// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QString>

#include "tournament.h"

/*!
 * \class Event
 * \inmodule libtournament
 * \inheaderfile libtournament/event.h
 */
class Event : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

public:
    explicit Event(const QString &fileName = {});

    ~Event();

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
    int numberOfTournaments();

    /*!
     * Returns the tournament with index \a index.
     */
    Tournament *getTournament(uint index);

    /*!
     * Returns a new tournament.
     */
    Tournament *createTournament();

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

public Q_SLOTS:
    void setFileName(const QString &fileName);

Q_SIGNALS:
    void fileNameChanged();

private:
    QSqlDatabase getDB();
    bool openDatabase(const QString &fileName);
    void closeDatabase();
    void createTables();
    int getDBVersion();
    void setDBVersion(int version);
    void loadTournaments();

    QString m_connName;
    QString m_fileName;

    std::vector<std::unique_ptr<Tournament>> m_tournaments;

    friend class Tournament;
};
