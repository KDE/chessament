// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.h"
#include "pairing.h"

using namespace Qt::Literals::StringLiterals;

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_playersModel(new PlayersModel(this))
    , m_pairingModel(new PairingModel(this))
    , m_standingsModel(new StandingsModel(this))
    , m_account(new Account)
{
    connect(m_playersModel, &PlayersModel::playerChanged, this, [this](Player *player) {
        m_tournament->savePlayer(player);
    });
}

Event *Controller::getEvent() const
{
    return m_event;
}

void Controller::setEvent(Event *event)
{
    if (m_event == event) {
        return;
    }
    m_event = event;
    Q_EMIT eventChanged();
}

Tournament *Controller::tournament() const
{
    return m_tournament;
}

void Controller::setTournament(Tournament *tournament)
{
    if (m_tournament == tournament) {
        return;
    }
    m_tournament = tournament;

    m_playersModel->setPlayers(m_tournament->players());
    m_pairingModel->setPairings(m_tournament->getPairings(1));
    m_standingsModel->setTournament(m_tournament);

    setHasOpenTournament(true);
    setCurrentPlayerByIndex(-1);
    setCurrentRound(1);

    Q_EMIT tournamentChanged();
}

bool Controller::hasOpenTournament()
{
    return m_hasOpenTournament;
}

void Controller::setHasOpenTournament(bool hasOpenTournament)
{
    if (m_hasOpenTournament == hasOpenTournament) {
        return;
    }
    m_hasOpenTournament = hasOpenTournament;
    Q_EMIT hasOpenTournamentChanged();
}

int Controller::currentPlayerIndex()
{
    return m_currentPlayerIndex;
}

void Controller::setCurrentPlayerByIndex(int currentPlayerIndex)
{
    if (m_currentPlayerIndex == currentPlayerIndex) {
        return;
    }
    m_currentPlayerIndex = currentPlayerIndex;
    if (m_currentPlayerIndex >= 0) {
        m_currentPlayer = m_tournament->players()->at(m_currentPlayerIndex);
    } else {
        m_currentPlayer = nullptr;
    }
    Q_EMIT currentPlayerChanged();
}

Player *Controller::currentPlayer() const
{
    return m_currentPlayer;
}

void Controller::setCurrentPlayer(Player *currentPlayer)
{
    if (m_currentPlayer == currentPlayer) {
        return;
    }
    m_currentPlayer = currentPlayer;
    m_currentPlayerIndex = m_tournament->players()->indexOf(m_currentPlayer);
    Q_EMIT currentPlayerChanged();
}

int Controller::currentRound()
{
    return m_currentRound;
}

bool Controller::hasCurrentRoundFinished()
{
    return m_tournament->isRoundFinished(m_tournament->currentRound());
}

void Controller::setCurrentRound(int currentRound)
{
    if (m_currentRound == currentRound) {
        return;
    }
    m_currentRound = currentRound;
    m_pairingModel->setPairings(m_tournament->getPairings(currentRound));
    Q_EMIT currentRoundChanged();
}

void Controller::importTrf(const QUrl &fileUrl)
{
    auto event = new Event();
    auto tournament = event->importTournament(fileUrl.toLocalFile());

    if (tournament.has_value()) {
        setEvent(event);
        setTournament(*tournament);

        setCurrentView(u"PlayersPage"_s);
    } else {
        setError(tournament.error());
    }
}

void Controller::exportTrf(const QUrl &fileUrl)
{
    if (!m_tournament->exportTrf(fileUrl.toLocalFile())) {
        setError(i18n("Couldn't export tournament."));
    }
}

QCoro::Task<void> Controller::pairRound()
{
    const auto pairings = co_await m_tournament->pairNextRound();

    if (!pairings.has_value()) {
        setError(pairings.error());
    }

    setCurrentRound(m_tournament->currentRound());
    m_pairingModel->setPairings(m_tournament->getPairings(m_currentRound));

    Q_EMIT hasCurrentRoundFinishedChanged();
}

QString Controller::getPlayersListDocument()
{
    return m_tournament->getPlayersListDocument();
}

void Controller::addPlayer(const QString &title,
                           const QString &name,
                           int rating,
                           int nationalRating,
                           const QString &playerId,
                           const QString &birthDate,
                           const QString &origin,
                           const QString &sex)
{
    auto startingRank = m_tournament->players()->size() + 1;
    auto player = new Player(startingRank, Player::titleForString(title), name, {}, rating, nationalRating, playerId, birthDate, {}, origin, sex);

    m_tournament->addPlayer(player);
    m_playersModel->addPlayer(player);
}

void Controller::savePlayer()
{
    m_playersModel->updatePlayer(m_currentPlayerIndex, m_currentPlayer);
}

bool Controller::setResult(int board, Qt::Key key)
{
    Pairing::Result result;

    switch (key) {
    case Qt::Key_0:
        result = {Pairing::PartialResult::Lost, Pairing::PartialResult::Win};
        break;
    case Qt::Key_1:
        result = {Pairing::PartialResult::Win, Pairing::PartialResult::Lost};
        break;
    case Qt::Key_5:
        result = {Pairing::PartialResult::Draw, Pairing::PartialResult::Draw};
        break;
    default:
        // Enter key changes to the next pairing
        return key == Qt::Key_Enter;
    }

    return setResult(board, result.first, result.second);
}

bool Controller::setResult(int board, Pairing::PartialResult whiteResult, Pairing::PartialResult blackResult)
{
    auto round = m_tournament->rounds().at(m_currentRound - 1);
    auto pairing = round->getPairing(board);

    Pairing::Result result = {whiteResult, blackResult};

    m_tournament->setResult(pairing, result);
    m_pairingModel->updatePairing(board);

    Q_EMIT hasCurrentRoundFinishedChanged();

    return true;
}

void Controller::newTournament(const QUrl &fileUrl, const QString &name, int numberOfRounds)
{
    auto event = new Event(fileUrl.toLocalFile());
    auto tournament = event->createTournament();
    tournament->setName(name);
    tournament->setNumberOfRounds(numberOfRounds);

    setEvent(event);
    setTournament(tournament);
    setCurrentView(u"PlayersPage"_s);
}

void Controller::openEvent(const QUrl &fileUrl)
{
    auto event = new Event(fileUrl.toLocalFile());

    setEvent(event);
    setTournament(event->tournaments().first());
    setCurrentView(u"PlayersPage"_s);
}

void Controller::saveEventAs(const QUrl &fileUrl)
{
    m_event->saveAs(fileUrl.toLocalFile());
    openEvent(fileUrl);
}

PlayersModel *Controller::playersModel() const
{
    return m_playersModel;
}

PairingModel *Controller::pairingModel() const
{
    return m_pairingModel;
}

StandingsModel *Controller::standingsModel() const
{
    return m_standingsModel;
}

Account *Controller::account() const
{
    return m_account;
}

void Controller::connectAccount()
{
    m_account->login();
}

QString Controller::currentView() const
{
    return m_currentView;
}

void Controller::setCurrentView(const QString &currentView)
{
    if (m_currentView == currentView) {
        return;
    }
    m_currentView = currentView;
    Q_EMIT currentViewChanged();
}

QString Controller::error() const
{
    return m_error;
}

void Controller::setError(const QString &error)
{
    m_error = error;
    Q_EMIT errorChanged();
}
