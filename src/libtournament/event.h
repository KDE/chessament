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
    Q_PROPERTY(QList<Tournament *> tournaments READ tournaments)

public:
    explicit Event(const QString &fileName = {});

    QString fileName();
    QList<Tournament *> tournaments();

    Tournament *createTournament();
    std::expected<Tournament *, QString> importTournament(const QString &fileName);
    void saveAs(const QString &fileName);

public Q_SLOTS:
    void setFileName(const QString &fileName);

Q_SIGNALS:
    void fileNameChanged();

private:
    QSqlDatabase getDB();
    bool openDatabase(const QString &fileName);
    void createTables();
    int getDBVersion();
    void setDBVersion(int version);
    void loadTournaments();

    QString m_connName;
    QString m_fileName;

    QList<Tournament *> m_tournaments;

    friend class Tournament;
};
