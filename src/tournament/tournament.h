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

#include "pairingengine.h"
#include "player.h"
#include "round.h"
#include "standing.h"
#include "tiebreaks/tiebreak.h"
#include "trf/writer.h"

class Document;
class Event;

using namespace Qt::StringLiterals;

/*!
 * \class Tournament
 * \inmodule tournament
 * \inheaderfile tournament/tournament.h
 */
class Tournament : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString federation READ federation WRITE setFederation NOTIFY federationChanged)
    Q_PROPERTY(QString chiefArbiter READ chiefArbiter WRITE setChiefArbiter NOTIFY chiefArbiterChanged)
    Q_PROPERTY(QString deputyChiefArbiter READ deputyChiefArbiter WRITE setDeputyChiefArbiter NOTIFY deputyChiefArbiterChanged)
    Q_PROPERTY(QString timeControl READ timeControl WRITE setTimeControl NOTIFY timeControlChanged)

    Q_PROPERTY(int numberOfPlayers READ numberOfPlayers NOTIFY numberOfPlayersChanged)
    Q_PROPERTY(int numberOfRatedPlayers READ numberOfRatedPlayers NOTIFY numberOfRatedPlayersChanged)
    Q_PROPERTY(int numberOfRounds READ numberOfRounds WRITE setNumberOfRounds NOTIFY numberOfRoundsChanged)
    Q_PROPERTY(int currentRound READ currentRound WRITE setCurrentRound NOTIFY currentRoundChanged)

public:
    /*!
     * \property Tournament::id
     * \brief the ID of the tournament
     *
     * This property holds the database ID of the tournament.
     */
    [[nodiscard]] QString id() const;

    /*!
     * \property Tournament::name
     * \brief the name of the tournament
     *
     * This property holds the name of the tournament.
     */
    [[nodiscard]] QString name() const;

    /*!
     * \property Tournament::city
     * \brief the location of the tournament
     *
     * This property holds the location of the tournament.
     */
    [[nodiscard]] QString city() const;

    /*!
     * \property Tournament::federation
     * \brief the federation of the tournament
     *
     * This property holds the federation of the tournament.
     */
    [[nodiscard]] QString federation() const;

    /*!
     * \property Tournament::chiefArbiter
     * \brief the Chief Arbiter of the tournament
     *
     * This property holds the Chief Arbiter of the tournament.
     */
    [[nodiscard]] QString chiefArbiter() const;

    /*!
     * \property Tournament::deputyChiefArbiter
     * \brief the Deputy Chief Arbiter of the tournament
     *
     * This property holds the Deputy Chief Arbiter of the tournament.
     */
    [[nodiscard]] QString deputyChiefArbiter() const;

    /*!
     * \property Tournament::timeControl
     * \brief the time control of the tournament
     *
     * This property holds the time control of the tournament.
     */
    [[nodiscard]] QString timeControl() const;

    /*!
     * \property Tournament::tiebreaks
     * \brief the tiebreaks of the tournament
     *
     * This property holds the list of tiebreaks of the tournament.
     */
    [[nodiscard]] std::vector<std::unique_ptr<Tiebreak>> &tiebreaks();

    /*!
     * \property Tournament::numberOfRounds
     * \brief the number of rounds of the tournament
     *
     * This property holds the number of rounds of the tournament.
     */
    [[nodiscard]] int numberOfRounds() const;

    /*!
     * \property Tournament::currentRound
     * \brief the number of the current round of the tournament
     *
     * The current round is the number of the last paired round, or 0 if the tournament has not started yet.
     */
    [[nodiscard]] int currentRound() const;

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
     * Saves \a player to the database.
     */
    void savePlayer(Player *player);

    /*!
     * Sorts players.
     */
    void sortPlayers();

    /*!
     * Changes the starting rank of \a player to \a startingRank.
     *
     * It adjusts the starting rank of other players if necessary.
     */
    void changePlayerStartingRank(Player *player, int startingRank);

    /*!
     * Returns the players grouped by their starting rank.
     */
    QMap<uint, Player *> getPlayersByStartingRank();

    /*!
     * Returns the players grouped by their id.
     */
    QMap<uint, Player *> getPlayersById();

    /*!
     * Returns the pairings of each player.
     *
     * \a maxRound The number of the highest round to consider. Negative number to use all rounds.
     */
    QHash<Player *, QList<Pairing *>> getPairingsByPlayer(int maxRound = -1);

    /*
     * Returns the standings of the tournament.
     *
     * \a state Helper object to compute the standings.
     */
    QList<Standing> getStandings(const State &state);

    Q_INVOKABLE QList<QVariantMap> availableTiebreaks();

    static std::unique_ptr<Tiebreak> tiebreak(const QString &id);

    void setTiebreaks(std::vector<std::unique_ptr<Tiebreak>> tiebreaks);

    [[nodiscard]] Round *round(int number) const;

    std::expected<void, QString> ensureRoundExists(int round);

    std::expected<void, QString> saveRound(Round *round);

    /*!
     * Adds the \a pairing to the round with number \a roundNumber.
     */
    std::expected<void, QString> addPairing(int roundNumber, std::unique_ptr<Pairing> pairing);

    /*!
     * Saves \a pairing to the database.
     */
    std::expected<void, QString> savePairing(Pairing *pairing);

    /*!
     * Sets the \a result to the the \a pairing and saves it to the database.
     *
     * \sa savePairing()
     */
    std::expected<void, QString> setResult(Pairing *pairing, std::pair<Pairing::PartialResult, Pairing::PartialResult> result);

    /*!
     * Returns the pairing corresponding to the \a round and \a board.
     */
    Pairing *getPairing(int round, int board);

    /*!
     * Returns the pairing of the \a round.
     */
    [[nodiscard]] QList<Pairing *> getPairings(int round) const;

    /*!
     * Sorts the pairings of all rounds.
     */
    std::expected<void, QString> sortPairings();

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

    /*!
     * Removes the pairing from \a round and subsequent rounds.
     *
     * \a keepByes Whether to keep byes.
     */
    std::expected<void, QString> removePairings(int round, bool keepByes);

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
    State getState(int maxRound = -1);

    /*!
     * \property Tournament::numberOfPlayers
     * \brief the number of players in the tournament
     *
     * This property holds the number of players in the tournament.
     */
    int numberOfPlayers();

    /*!
     * Returns the number of rated players in the tournament.
     */
    int numberOfRatedPlayers();

    void saveTiebreaks();

    /*!
     * Returns the value of the option \a name
     *
     * \sa setOption()
     */
    QVariant getOption(const QString &name);

    /*!
     * Sets the option \a name to \a value.
     *
     * \sa getOption()
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
    QString toTrf(TrfWriter::Options options = {}, int maxRound = -1);

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
    void setCity(const QString &city);
    void setFederation(const QString &federation);
    void setChiefArbiter(const QString &chiefArbiter);
    void setDeputyChiefArbiter(const QString &deputyChiefArbiter);
    void setTimeControl(const QString &timeControl);
    void setNumberOfRounds(int numberOfRounds);
    void setCurrentRound(int currentRound);

    void setInitialColor(Tournament::InitialColor color);

Q_SIGNALS:
    void idChanged();
    void nameChanged();
    void cityChanged();
    void federationChanged();
    void chiefArbiterChanged();
    void deputyChiefArbiterChanged();
    void timeControlChanged();
    void tiebreaksChanged();

    void numberOfPlayersChanged();
    void numberOfRatedPlayersChanged();
    void numberOfRoundsChanged();
    void currentRoundChanged();

private:
    explicit Tournament(Event *event);

    std::expected<void, QString> createNewTournament();
    std::expected<void, QString> loadTournament(const QString &id = {});
    void loadOptions();
    std::expected<void, QString> loadPlayers();
    std::expected<void, QString> loadRounds();
    std::expected<void, QString> loadPairings();
    std::expected<void, QString> loadTiebreaks();

    Event *m_event;

    QString m_id;
    QString m_name;
    QString m_city;
    QString m_federation;
    QString m_chiefArbiter;
    QString m_deputyChiefArbiter;
    QString m_timeControl;
    int m_numberOfRounds = 1;
    int m_currentRound = 0;
    std::vector<std::unique_ptr<Tiebreak>> m_tiebreaks;

    std::vector<std::unique_ptr<Player>> m_players;
    std::vector<std::unique_ptr<Round>> m_rounds;

    Tournament::InitialColor m_initialColor;

    friend class Event;
    friend class TrfReader;
    friend class TrfWriter;
};
