// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
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
#include "tiebreaks/tiebreak.h"

class Document;
class Event;

using namespace Qt::StringLiterals;

/*!
 * \class Tournament
 * \inmodule libtournament
 * \inheaderfile libtournament/tournament.h
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
    Q_PROPERTY(QList<Tiebreak *> tiebreaks READ tiebreaks WRITE setTiebreaks NOTIFY tiebreaksChanged)

    Q_PROPERTY(int numberOfPlayers READ numberOfPlayers NOTIFY numberOfPlayersChanged)
    Q_PROPERTY(int numberOfRatedPlayers READ numberOfRatedPlayers NOTIFY numberOfRatedPlayersChanged)
    Q_PROPERTY(int numberOfRounds READ numberOfRounds WRITE setNumberOfRounds NOTIFY numberOfRoundsChanged)
    Q_PROPERTY(int currentRound READ currentRound WRITE setCurrentRound NOTIFY currentRoundChanged)
    Q_PROPERTY(QList<Tiebreak *> tiebreaks READ tiebreaks WRITE setTiebreaks NOTIFY tiebreaksChanged)

public:
    /*!
     * \property Tournament::id
     * \brief the ID of the tournament
     *
     * This property holds the database ID of the tournament.
     */
    QString id() const;

    /*!
     * \property Tournament::name
     * \brief the name of the tournament
     *
     * This property holds the name of the tournament.
     */
    QString name() const;

    /*!
     * \property Tournament::city
     * \brief the location of the tournament
     *
     * This property holds the location of the tournament.
     */
    QString city() const;

    /*!
     * \property Tournament::federation
     * \brief the federation of the tournament
     *
     * This property holds the federation of the tournament.
     */
    QString federation() const;

    /*!
     * \property Tournament::chiefArbiter
     * \brief the Chief Arbiter of the tournament
     *
     * This property holds the Chief Arbiter of the tournament.
     */
    QString chiefArbiter() const;

    /*!
     * \property Tournament::deputyChiefArbiter
     * \brief the Deputy Chief Arbiter of the tournament
     *
     * This property holds the Deputy Chief Arbiter of the tournament.
     */
    QString deputyChiefArbiter() const;

    /*!
     * \property Tournament::timeControl
     * \brief the time control of the tournament
     *
     * This property holds the time control of the tournament.
     */
    QString timeControl() const;

    /*!
     * \property Tournament::tiebreaks
     * \brief the tiebreaks of the tournament
     *
     * This property holds the list of tiebreaks of the tournament.
     */
    QList<Tiebreak *> tiebreaks() const;

    /*!
     * \property Tournament::numberOfRounds
     * \brief the number of rounds of the tournament
     *
     * This property holds the number of rounds of the tournament.
     */
    int numberOfRounds() const;

    /*!
     * \property Tournament::currentRound
     * \brief the number of the current round of the tournament
     *
     * The current round is the number of the last paired round, or 0 if the tournament has not started yet.
     */
    int currentRound() const;

    Event *getEvent() const;

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
    QList<PlayerTiebreaks> getStandings(State state);

    Q_INVOKABLE QList<QVariantMap> availableTiebreaks()
    {
        return {
            {
                {"id"_L1, "pts"_L1},
                {"name"_L1, i18nc("Game points", "Points")},
            },
            {
                {"id"_L1, "bh"_L1},
                {"name"_L1, i18nc("Buchholz tiebreak", "Buchholz")},
            },
        };
    }

    /*!
     * Returns the rounds of the tournament.
     */
    std::vector<std::unique_ptr<Round>> rounds() const;

    /*!
     * Adds the \a pairing to the round \a round.
     */
    std::expected<void, QString> addPairing(int round, std::unique_ptr<Pairing> pairing);

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

    enum class ReportField {
        Player = 1,
        TournamentName,
        City,
        Federation,
        StartDate,
        EndDate,
        NumberOfPlayers,
        NumberOfRatedPlayers,
        NumberOfTeams,
        TournamentType,
        ChiefArbiter,
        DeputyChiefArbiter,
        TimeControl,
        Calendar,
        Unknown,
    };
    Q_ENUM(ReportField)

    static QString reportFieldString(ReportField field)
    {
        switch (field) {
        case ReportField::Player:
            return QStringLiteral("001");
        case ReportField::TournamentName:
            return QStringLiteral("012");
        case ReportField::City:
            return QStringLiteral("022");
        case ReportField::Federation:
            return QStringLiteral("032");
        case ReportField::StartDate:
            return QStringLiteral("042");
        case ReportField::EndDate:
            return QStringLiteral("052");
        case ReportField::NumberOfPlayers:
            return QStringLiteral("062");
        case ReportField::NumberOfRatedPlayers:
            return QStringLiteral("072");
        case ReportField::NumberOfTeams:
            return QStringLiteral("082");
        case ReportField::TournamentType:
            return QStringLiteral("092");
        case ReportField::ChiefArbiter:
            return QStringLiteral("102");
        case ReportField::DeputyChiefArbiter:
            return QStringLiteral("112");
        case ReportField::TimeControl:
            return QStringLiteral("122");
        case ReportField::Calendar:
            return QStringLiteral("132");
        default:
            return {};
        }
    };

    static ReportField reportFieldForString(QStringView number)
    {
        if (number == QStringLiteral("001")) {
            return ReportField::Player;
        } else if (number == QStringLiteral("012")) {
            return ReportField::TournamentName;
        } else if (number == QStringLiteral("022")) {
            return ReportField::City;
        } else if (number == QStringLiteral("032")) {
            return ReportField::Federation;
        } else if (number == QStringLiteral("042")) {
            return ReportField::StartDate;
        } else if (number == QStringLiteral("052")) {
            return ReportField::EndDate;
        } else if (number == QStringLiteral("062")) {
            return ReportField::NumberOfPlayers;
        } else if (number == QStringLiteral("072")) {
            return ReportField::NumberOfRatedPlayers;
        } else if (number == QStringLiteral("082")) {
            return ReportField::NumberOfTeams;
        } else if (number == QStringLiteral("092")) {
            return ReportField::TournamentType;
        } else if (number == QStringLiteral("102")) {
            return ReportField::ChiefArbiter;
        } else if (number == QStringLiteral("112")) {
            return ReportField::DeputyChiefArbiter;
        } else if (number == QStringLiteral("122")) {
            return ReportField::TimeControl;
        } else if (number == QStringLiteral("132")) {
            return ReportField::Calendar;
        }
        return ReportField::Unknown;
    }

    /*!
     * \enum Tournament::TrfOption
     *
     * This enum type specifies the options for the Tournament Report File export.
     *
     * \value NumberOfRounds Adds the number of rounds of the tournament for pairing engines.
     * \value InitialColorWhite Adds an indication for pairing engines that the first player had the white pieces in the first round.
     * \value InitialColorBlack Adds an indication for pairing engines that the first player had the black pieces in the first round.
     *
     * \sa toTrf(), exportTrf()
     */
    enum class TrfOption {
        NumberOfRounds = 0x1,
        InitialColorWhite = 0x2,
        InitialColorBlack = 0x4,
    };
    Q_DECLARE_FLAGS(TrfOptions, TrfOption)

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
    QJsonObject toJson() const;

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
    QString toTrf(TrfOptions options = {}, int maxRound = -1);

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
    void setTiebreaks(const QList<Tiebreak *> &tiebreaks);

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
    QList<Tiebreak *> m_tiebreaks;

    std::vector<std::unique_ptr<Player>> m_players;
    std::vector<std::unique_ptr<Round>> m_rounds;

    Tournament::InitialColor m_initialColor;

    friend class Event;
    friend class TRFReader;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Tournament::TrfOptions)
