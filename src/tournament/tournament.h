// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <KLocalizedString>
#include <QCoroTask>
#include <QFile>
#include <QFlags>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTextStream>

#include <expected>

#include "arbiter.h"
#include "pairingengine.h"
#include "player.h"
#include "round.h"
#include "standing.h"
#include "tiebreaks/tiebreak.h"
#include "tiebreaks/tiebreaks.h"
#include "timecontrol.h"
#include "trf/trf.h"

class Document;
class Event;

using namespace Qt::StringLiterals;

/*!
 * \class Tournament
 * \inmodule libtournament
 * \inheaderfile tournament/tournament.h
 */
class Tournament : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(QString federation READ federation WRITE setFederation NOTIFY federationChanged)

    Q_PROPERTY(int numberOfPlayers READ numberOfPlayers NOTIFY numberOfPlayersChanged)
    Q_PROPERTY(int numberOfRatedPlayers READ numberOfRatedPlayers NOTIFY numberOfRatedPlayersChanged)
    Q_PROPERTY(int numberOfRounds READ numberOfRounds WRITE setNumberOfRounds NOTIFY numberOfRoundsChanged)
    Q_PROPERTY(int currentRound READ currentRound WRITE setCurrentRound NOTIFY currentRoundChanged)

public:
    /*!
     * \property Tournament::id
     *
     * This property holds the database ID of the tournament.
     */
    [[nodiscard]] QString id() const;

    /*!
     * \property Tournament::name
     *
     * This property holds the name of the tournament.
     */
    [[nodiscard]] QString name() const;

    /*!
     * \property Tournament::location
     *
     * This property holds the location of the tournament.
     */
    [[nodiscard]] QString location() const;

    /*!
     * \property Tournament::federation
     *
     * This property holds the federation of the tournament.
     */
    [[nodiscard]] QString federation() const;

    /*!
     * Returns the arbiters of the tournament.
     */
    [[nodiscard]] Arbiters &arbiters();

    void saveArbiters();

    /*!
     * Returns the time control of the tournament.
     */
    TimeControl &timeControl();

    void saveTimeControl();

    /*!
     * Returns the tiebreaks of the tournament.
     */
    [[nodiscard]] Tiebreaks &tiebreaks();

    /*!
     * \property Tournament::numberOfRounds
     *
     * This property holds the number of rounds of the tournament.
     */
    [[nodiscard]] int numberOfRounds() const;

    /*!
     * \property Tournament::currentRound
     *
     * This property holds the current round of the tournament. It is 0 if the tournament has not started yet.
     */
    [[nodiscard]] int currentRound() const;

    /*!
     * Returns the event that holds this tournament.
     */
    [[nodiscard]] Event *getEvent() const;

    /*!
     * Returns the players of the tournament.
     */
    [[nodiscard]] QList<Player *> players() const;

    /*!
     * Adds \a player to the tournament.
     */
    std::expected<void, QString> addPlayer(std::unique_ptr<Player> player);

    /*!
     * Removes the player with starting rank \a startingRank.
     */
    std::expected<void, QString> deletePlayer(int startingRank);

    /*!
     * Saves \a player to the database.
     */
    void savePlayer(Player *player);

    /*!
     * Sorts players.
     */
    void sortPlayers();

    void updateRatings(int listId);

    /*!
     * Changes the starting rank of \a player to \a startingRank.
     *
     * The starting rank of other players is adjusted if necessary.
     */
    int changePlayerStartingRank(Player *player, int startingRank);

    /*!
     * Returns the players grouped by their starting rank.
     */
    QMap<uint, Player *> playersByStartingRank();

    /*!
     * Returns the players grouped by their database ID.
     */
    QMap<QString, Player *> playersById();

    /*!
     * Returns the pairings of each player.
     *
     * \a maxRound The number of the highest round to consider. Negative number to use all rounds.
     */
    QHash<Player *, QList<Pairing *>> pairingsByPlayer(std::optional<int> maxRound = std::nullopt);

    /*!
     * Returns the pairings of \a player.
     */
    QList<Pairing *> pairingsOfPlayer(Player *player);

    /*
     * Returns the standings of the tournament.
     *
     * \a state Helper object to compute the standings.
     */
    QList<Standing> standings(const State &state);

    Q_INVOKABLE QList<QVariantMap> availableTiebreaks();

    std::expected<void, QString> setTiebreaksFromTrf(const QString &line);

    [[nodiscard]] Round *round(int number) const;

    std::expected<void, QString> ensureRoundExists(int round);

    std::expected<void, QString> saveRound(Round *round);

    QMap<int, Round *> roundsById();

    /*!
     * Adds the \a pairing to the round with number \a roundNumber.
     */
    std::expected<void, QString> addPairing(int roundNumber, std::unique_ptr<Pairing> pairing);

    /*!
     * Saves \a pairing to the database.
     */
    std::expected<void, QString> savePairing(Pairing *pairing, int roundNumber = 0);

    /*!
     * Sets the \a result to the the \a pairing and saves it to the database.
     *
     * \sa savePairing()
     */
    std::expected<void, QString> setResult(Pairing *pairing, std::pair<Pairing::PartialResult, Pairing::PartialResult> result);

    std::expected<void, QString> setBye(Player *player, int round, Pairing::PartialResult result);

    bool hasRetired(Player *player);

    /*!
     * Sets a zero-point bye for all upcoming rounds for \a player.
     */
    std::expected<void, QString> retire(Player *player);

    /*!
     * Removes all upcoming byes of \a player.
     */
    std::expected<void, QString> incorpore(Player *player);

    /*!
     * Returns a list of players that have a voluntary bye in \a round.
     *
     * \sa Pairing::isVoluntaryBye()
     */
    Q_INVOKABLE [[nodiscard]] QList<Player *> voluntaryByes(int round) const;

    /*!
     * Returns the pairing corresponding to the \a round and \a board.
     */
    Pairing *pairing(int round, int board);

    /*!
     * Returns the pairing of \a player in round \a round.
     */
    Pairing *pairing(int round, Player *player) const;

    /*!
     * Returns the pairing of the \a round.
     */
    [[nodiscard]] QList<Pairing *> pairings(int round) const;

    /*!
     * Sorts the pairings of round \a round.
     *
     * If \a round is null, it sorts the pairings of all rounds.
     */
    std::expected<void, QString> sortPairings(std::optional<int> round = std::nullopt);

    /*!
     * Returns whether \a round has finished.
     *
     * A round has finished if all its pairings have a result.
     */
    Q_INVOKABLE bool isRoundFinished(int round);

    /*!
     * Returns whether \a round is fully paired.
     */
    bool isRoundFullyPaired(int round);

    QCoro::Task<std::expected<QList<std::pair<uint, uint>>, QString>> calculatePairings(int round);

    /*!
     * Pairs the next round of the tournament.
     *
     * The caller must ensure that the current round has finished and the next one is ready to be paired.
     */
    QCoro::Task<std::expected<bool, QString>> pairNextRound();

    std::expected<void, QString> removePairing(int round, Pairing *pairing);

    /*!
     * Removes the pairing from \a round and subsequent rounds.
     *
     * \a keepByes Whether to keep byes.
     */
    std::expected<void, QString> removePairings(int round, bool keepByes);

    /*!
     * Removes all pairings of \a player.
     */
    std::expected<void, QString> deletePairings(Player *player);

    /*!
     * \enum Tournament::InitialColor
     *
     * This enum type represents the piece color of the first player of the tournament for the first round.
     *
     * \value White White pieces.
     * \value Black Black pieces.
     */
    enum class InitialColor {
        White,
        Black,
    };
    Q_ENUM(InitialColor);

    /*!
     * Returns the initial color of the tournament.
     */
    InitialColor initialColor();

    /*!
     * Returns a helper object.
     *
     * \a maxRound The number of the highest round to include.
     */
    State state(std::optional<int> maxRound = std::nullopt);

    /*!
     * \property Tournament::numberOfPlayers
     *
     * This property holds the number of players in the tournament.
     */
    int numberOfPlayers();

    /*!
     * \property Tournament::numberOfRatedPlayers
     *
     * This property holds the number of rated players in the tournament.
     */
    int numberOfRatedPlayers();

    void saveTiebreaks();

    /*!
     * Returns the value of the option \a name
     *
     * \sa setOption()
     */
    QVariant option(const QString &name);

    /*!
     * Sets the option \a name to \a value.
     *
     * \sa option()
     */
    void setOption(const QString &name, const QVariant &value);

    /*!
     * Returns a JSON represetation of the tournament.
     *
     * \sa read()
     */
    [[nodiscard]] QJsonObject toJson() const;

    /*!
     * Loads the tournament from its JSON represetation.
     *
     * \a json The JSON represetation.
     *
     * \sa toJson()
     */
    void read(const QJsonObject &json);

    /*!
     * Returns the Tournament Report File (TRF).
     *
     * \a options
     *
     * \a maxRound The number of the highest round to include. Negative number to means to include all rounds.
     *
     * \sa exportTrf()
     */
    QString toTrf(Trf::Options options = {}, std::optional<int> maxRound = std::nullopt);

    /*!
     * Imports the tournament from a Tournament Report File (TRF).
     *
     * \a trf The content of the TRF.
     *
     * Returns true if successful; false otherwise.
     */
    std::expected<void, QString> readTrf(QTextStream trf);

    /*!
     * Imports the tournament from a Tournament Report File (TRF).
     *
     * \a fileName The file name of the TRF.
     *
     * Returns true if successful; false otherwise.
     */
    std::expected<void, QString> loadTrf(const QString &fileName);

    /*!
     * Saves the Tournament Report File (TRF) to a file.
     *
     * Returns true if success; false otherwise.
     *
     * \a fileName The file name.
     *
     * \sa toTrf()
     */
    bool exportTrf(const QString &fileName);

public Q_SLOTS:
    void setId(const QString &id);
    void setName(const QString &name);
    void setLocation(const QString &location);
    void setFederation(const QString &federation);
    void setNumberOfRounds(int numberOfRounds);
    void setCurrentRound(int currentRound);

    void setInitialColor(Tournament::InitialColor color);

Q_SIGNALS:
    void idChanged();
    void nameChanged();
    void locationChanged();
    void federationChanged();

    void numberOfPlayersChanged();
    void numberOfRatedPlayersChanged();
    void numberOfRoundsChanged();
    void currentRoundChanged();

    void optionChanged();

    void playerChanged();

    void pairingChanged();

private:
    explicit Tournament(Event *event);

    std::expected<void, QString> createNewTournament();
    std::expected<void, QString> loadTournament(const QString &id = {});
    std::expected<void, QString> loadOptions();
    std::expected<void, QString> loadPlayers();
    std::expected<void, QString> loadRounds();
    std::expected<void, QString> loadPairings();
    std::expected<void, QString> loadTiebreaks();
    std::expected<void, QString> loadArbiters();
    std::expected<void, QString> loadTimeControl();

    Event *m_event;

    QString m_id;
    QString m_name;
    QString m_location;
    QString m_federation;
    Arbiters m_arbiters;
    TimeControl m_timeControl;
    int m_numberOfRounds = 1;
    int m_currentRound = 0;
    QVariantMap m_options;
    Tiebreaks m_tiebreaks;

    std::vector<std::unique_ptr<Player>> m_players;
    std::vector<std::unique_ptr<Round>> m_rounds;

    Tournament::InitialColor m_initialColor;

    friend class Event;
    friend class TrfReader;
    friend class TrfWriter;
};
