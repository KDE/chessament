// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCoroQmlTask>
#include <QObject>
#include <QTemporaryFile>

#include "account.h"
#include "pairingmodel.h"
#include "playersmodel.h"
#include "standingsmodel.h"
#include "tournament/event.h"
#include "tournament/tournament.h"

class Controller : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Event *event READ getEvent NOTIFY eventChanged)
    Q_PROPERTY(Tournament *tournament READ tournament WRITE setTournament NOTIFY tournamentChanged)
    Q_PROPERTY(bool hasOpenTournament READ hasOpenTournament WRITE setHasOpenTournament NOTIFY hasOpenTournamentChanged)
    Q_PROPERTY(Player *currentPlayer READ currentPlayer WRITE setCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int currentRound READ currentRound WRITE setCurrentRound NOTIFY currentRoundChanged)
    Q_PROPERTY(bool hasCurrentRoundFinished READ hasCurrentRoundFinished NOTIFY hasCurrentRoundFinishedChanged)
    Q_PROPERTY(bool areStandingsValid READ areStandingsValid WRITE setAreStandingsValid NOTIFY areStandingsValidChanged)

    Q_PROPERTY(PlayersModel *playersModel READ playersModel CONSTANT)
    Q_PROPERTY(PairingModel *pairingModel READ pairingModel CONSTANT)
    Q_PROPERTY(StandingsModel *standingsModel READ standingsModel CONSTANT)

    Q_PROPERTY(Account *account READ account CONSTANT)

    Q_PROPERTY(QString currentView READ currentView WRITE setCurrentView NOTIFY currentViewChanged)
    Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit Controller(QObject *parent = nullptr);

    [[nodiscard]] Event *getEvent() const;
    [[nodiscard]] Tournament *tournament() const;
    bool hasOpenTournament();
    [[nodiscard]] Player *currentPlayer() const;
    int currentRound();
    bool hasCurrentRoundFinished();
    bool areStandingsValid();

    void setEvent(std::unique_ptr<Event> event);
    Q_INVOKABLE void addPlayer(const QString &title,
                               const QString &name,
                               int rating,
                               int nationalRating,
                               const QString &playerId,
                               const QString &birthDate,
                               const QString &origin,
                               const QString &sex);
    Q_INVOKABLE void sortPlayers();
    Q_INVOKABLE bool setResult(int board, Qt::Key key);
    Q_INVOKABLE bool setResult(int board, Pairing::PartialResult whiteResult, Pairing::PartialResult blackResult);
    Q_INVOKABLE void newTournament(const QUrl &fileUrl, const QString &name, int numberOfRounds);
    Q_INVOKABLE void openEvent(const QUrl &fileUrl);
    Q_INVOKABLE void saveEventAs(const QUrl &fileUrl);
    Q_INVOKABLE void importTrf(const QUrl &fileUrl);
    Q_INVOKABLE void exportTrf(const QUrl &fileUrl);
    Q_INVOKABLE QCoro::Task<void> pairRound(bool sort, uint color);
    Q_INVOKABLE void removePairings(bool keepByes);
    Q_INVOKABLE QCoro::QmlTask reloadStandings(int maxRound);

    std::unique_ptr<Document> playersDocument();
    Q_INVOKABLE void savePlayersDocument(const QString &fileName);
    Q_INVOKABLE void printPlayersDocument();
    std::unique_ptr<Document> pairingsDocument();
    Q_INVOKABLE void savePairingsDocument(const QString &fileName);
    Q_INVOKABLE void printPairingsDocument();

    [[nodiscard]] PlayersModel *playersModel() const;
    [[nodiscard]] PairingModel *pairingModel() const;
    [[nodiscard]] StandingsModel *standingsModel() const;

    [[nodiscard]] Account *account() const;

    Q_INVOKABLE void connectAccount();

    [[nodiscard]] QString currentView() const;
    [[nodiscard]] QString error() const;

public Q_SLOTS:
    void setTournament(Tournament *tournament);
    void setHasOpenTournament(bool hasOpenTournament);
    void setCurrentPlayer(Player *currentPlayer);
    void setCurrentRound(int currentRound);
    void setAreStandingsValid(bool valid);
    void setCurrentView(const QString &currentView);
    void setError(const QString &error);

Q_SIGNALS:
    void eventChanged();
    void tournamentChanged();
    void hasOpenTournamentChanged();
    void currentPlayerChanged();
    void currentRoundChanged();
    void hasCurrentRoundFinishedChanged();
    void areStandingsValidChanged();
    void currentViewChanged();
    void errorChanged();

private:
    QCoro::Task<> updateStandings(int maxRound);

    std::unique_ptr<Event> m_event;
    Tournament *m_tournament;

    bool m_hasOpenTournament = false;
    int m_currentRound = 1;
    bool m_areStandingsValid = false;

    Player *m_currentPlayer = nullptr;
    PlayersModel *m_playersModel;
    PairingModel *m_pairingModel;
    StandingsModel *m_standingsModel;

    Account *m_account;

    QString m_currentView;
    QString m_error;

    std::unique_ptr<QTemporaryFile> m_tempfile;
};
