// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QString>

#include "tournament.h"

class Event : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

public:
    explicit Event(const QString &fileName = {});

    ~Event();

    QString fileName();
    int numberOfTournaments();
    Tournament *getTournament(uint index);

    Tournament *createTournament();
    std::expected<Tournament *, QString> importTournament(const QString &fileName);
    void saveAs(const QString &fileName);
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
