// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>

#include "account.h"
#include "libtournament/event.h"
#include "libtournament/tournament.h"
#include "pairingmodel.h"
#include "playersmodel.h"
#include "standingsmodel.h"

class Controller : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Event *event READ getEvent WRITE setEvent NOTIFY eventChanged)
    Q_PROPERTY(Tournament *tournament READ tournament WRITE setTournament NOTIFY tournamentChanged)
    Q_PROPERTY(bool hasOpenTournament READ hasOpenTournament WRITE setHasOpenTournament NOTIFY hasOpenTournamentChanged)
    Q_PROPERTY(int currentPlayerIndex READ currentPlayerIndex WRITE setCurrentPlayerByIndex NOTIFY currentPlayerChanged)
    Q_PROPERTY(Player *currentPlayer READ currentPlayer WRITE setCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int currentRound READ currentRound WRITE setCurrentRound NOTIFY currentRoundChanged)
    Q_PROPERTY(bool hasCurrentRoundFinished READ hasCurrentRoundFinished NOTIFY hasCurrentRoundFinishedChanged)

    Q_PROPERTY(PlayersModel *playersModel READ playersModel CONSTANT)
    Q_PROPERTY(PairingModel *pairingModel READ pairingModel CONSTANT)
    Q_PROPERTY(StandingsModel *standingsModel READ standingsModel CONSTANT)

    Q_PROPERTY(Account *account READ account CONSTANT)

    Q_PROPERTY(QString currentView READ currentView WRITE setCurrentView NOTIFY currentViewChanged)
    Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit Controller(QObject *parent = nullptr);

    Event *getEvent() const;
    Tournament *tournament() const;
    bool hasOpenTournament();
    int currentPlayerIndex();
    Player *currentPlayer() const;
    int currentRound();
    bool hasCurrentRoundFinished();

    Q_INVOKABLE void addPlayer(const QString &title,
                               const QString &name,
                               int rating,
                               int nationalRating,
                               const QString &playerId,
                               const QString &birthDate,
                               const QString &origin,
                               const QString &sex);
    Q_INVOKABLE void savePlayer();
    Q_INVOKABLE bool setResult(int board, Qt::Key key);
    Q_INVOKABLE bool setResult(int board, Pairing::PartialResult whiteResult, Pairing::PartialResult blackResult);
    Q_INVOKABLE void newTournament(const QUrl &fileUrl, const QString &name, int numberOfRounds);
    Q_INVOKABLE void openEvent(const QUrl &fileUrl);
    Q_INVOKABLE void saveEventAs(const QUrl &fileUrl);
    Q_INVOKABLE void importTrf(const QUrl &fileUrl);
    Q_INVOKABLE void exportTrf(const QUrl &fileUrl);
    Q_INVOKABLE QCoro::Task<void> pairRound();
    Q_INVOKABLE void removePairings(bool keepByes);

    Q_INVOKABLE QString getPlayersListDocument();

    PlayersModel *playersModel() const;
    PairingModel *pairingModel() const;
    StandingsModel *standingsModel() const;

    Account *account() const;

    Q_INVOKABLE void connectAccount();

    QString currentView() const;
    QString error() const;

public Q_SLOTS:
    void setEvent(Event *event);
    void setTournament(Tournament *tournament);
    void setHasOpenTournament(bool hasOpenTournament);
    void setCurrentPlayer(Player *currentPlayer);
    void setCurrentPlayerByIndex(int currentPlayer);
    void setCurrentRound(int currentRound);
    void setCurrentView(const QString &currentView);
    void setError(const QString &error);

Q_SIGNALS:
    void eventChanged();
    void tournamentChanged();
    void hasOpenTournamentChanged();
    void currentPlayerChanged();
    void currentRoundChanged();
    void hasCurrentRoundFinishedChanged();
    void currentViewChanged();
    void errorChanged();

private:
    Event *m_event;
    Tournament *m_tournament;

    bool m_hasOpenTournament = false;
    int m_currentPlayerIndex = -1;
    int m_currentRound = 1;

    Player *m_currentPlayer = nullptr;
    PlayersModel *m_playersModel;
    PairingModel *m_pairingModel;
    StandingsModel *m_standingsModel;

    Account *m_account;

    QString m_currentView;
    QString m_error;
};
