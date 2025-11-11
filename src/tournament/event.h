// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

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
    Tournament *getTournament(uint index);

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

public Q_SLOTS:
    void setFileName(const QString &fileName);

Q_SIGNALS:
    void fileNameChanged();

private:
    QSqlDatabase getDB();
    std::expected<void, QString> openDatabase();
    void closeDatabase();
    std::expected<void, QString> createTables();
    std::expected<int, QString> getDBVersion();
    std::expected<void, QString> setDBVersion(int version);
    std::expected<void, QString> loadTournaments();

    QString m_connName;
    QString m_fileName;

    std::vector<std::unique_ptr<Tournament>> m_tournaments;

    friend class Tournament;
};
