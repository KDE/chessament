// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QString>

using namespace Qt::StringLiterals;

/*!
 * \class Player
 * \inmodule libtournament
 * \inheaderfile libtournament/player.h
 */
class Player : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(int startingRank READ startingRank NOTIFY startingRankChanged)
    Q_PROPERTY(Title title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString surname READ surname WRITE setSurname NOTIFY surnameChanged)
    Q_PROPERTY(int rating READ rating WRITE setRating NOTIFY ratingChanged)
    Q_PROPERTY(int nationalRating READ nationalRating WRITE setNationalRating NOTIFY nationalRatingChanged)
    Q_PROPERTY(QString playerId READ playerId WRITE setPlayerId NOTIFY playerIdChanged)
    Q_PROPERTY(QString birthDate READ birthDate WRITE setBirthDate NOTIFY birthDateChanged)
    Q_PROPERTY(QString federation READ federation WRITE setFederation NOTIFY federationChanged)
    Q_PROPERTY(QString origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(QString sex READ sex WRITE setSex NOTIFY sexChanged)

public:
    /*!
     * \enum Player::Title
     *
     * This enum type specifies the title of a player.
     *
     * \value None No title.
     * \value GM Grandmaster.
     * \value IM International Master.
     * \value FM FIDE Master.
     * \value CM Candidate Master.
     * \value WGM Woman Grandmaster.
     * \value WIM Woman International Master.
     * \value WFM Woman FIDE Master.
     * \value WCM Woman Candidate Master.
     */
    enum class Title {
        None,
        GM,
        IM,
        FM,
        CM,
        WGM,
        WIM,
        WFM,
        WCM
    };
    Q_ENUM(Title)

    static constexpr uint titleStrengthLevel(Player::Title title)
    {
        switch (title) {
        case Title::GM:
            return 0;
        case Title::IM:
            return 1;
        case Title::WGM:
            return 2;
        case Title::FM:
            return 3;
        case Title::WIM:
            return 4;
        case Title::CM:
            return 5;
        case Title::WFM:
            return 6;
        case Title::WCM:
            return 7;
        case Title::None:
            return 8;
        }
        Q_UNREACHABLE();
    }

    static QString titleString(Player::Title title)
    {
        switch (title) {
        case Title::GM:
            return QStringLiteral("GM");
        case Title::IM:
            return QStringLiteral("IM");
        case Title::FM:
            return QStringLiteral("FM");
        case Title::CM:
            return QStringLiteral("CM");
        case Title::WGM:
            return QStringLiteral("WGM");
        case Title::WIM:
            return QStringLiteral("WIM");
        case Title::WFM:
            return QStringLiteral("WFM");
        case Title::WCM:
            return QStringLiteral("WCM");
        default:
            return {};
        }
    };

    static Title titleForString(const QString &title)
    {
        auto t = title.toLower();
        if (t == QStringLiteral("gm")) {
            return Title::GM;
        }
        if (t == QStringLiteral("im")) {
            return Title::IM;
        }
        if (t == QStringLiteral("fm")) {
            return Title::FM;
        }
        if (t == QStringLiteral("cm")) {
            return Title::CM;
        }
        if (t == QStringLiteral("wgm")) {
            return Title::WGM;
        }
        if (t == QStringLiteral("wim")) {
            return Title::WIM;
        }
        if (t == QStringLiteral("wfm")) {
            return Title::WFM;
        }
        if (t == QStringLiteral("wcm")) {
            return Title::WCM;
        }
        return Title::None;
    }

    explicit Player() = default;
    explicit Player(int startingRank, const QString &name, int rating);
    explicit Player(int startingRank,
                    Player::Title title,
                    const QString &name,
                    const QString &surname,
                    int rating,
                    int nationalRating,
                    const QString &playerId,
                    const QString &birthDate,
                    const QString &federation,
                    const QString &origin,
                    const QString &sex);

    /*!
     * \property Player::id
     * \brief the ID of the player
     *
     * This property holds the database ID of the player.
     */
    int id() const;

    /*!
     * \property Player::startingRank
     * \brief the ID of the player
     *
     * This property holds the starting rank of the player.
     */
    int startingRank() const;

    /*!
     * \property Player::title
     * \brief the title of the player
     *
     * This property holds the title of the player.
     */
    Title title() const;

    /*!
     * \property Player::name
     * \brief the name of the player
     *
     * This property holds the name of the player.
     */
    QString name() const;

    /*!
     * \property Player::surname
     * \brief the surname of the player
     *
     * This property holds the surname of the player.
     */
    QString surname() const;

    /*!
     * \property Player::rating
     * \brief the rating of the player
     *
     * This property holds the rating of the player.
     */
    int rating() const;

    /*!
     * \property Player::nationalRating
     * \brief the national rating of the player
     *
     * This property holds the national rating of the player.
     */
    int nationalRating() const;

    /*!
     * \property Player::playerId
     * \brief the player ID of the player
     *
     * This property holds the player ID of the player.
     */
    QString playerId() const;

    /*!
     * \property Player::birthDate
     * \brief the birth date of the player
     *
     * This property holds the birth date of the player.
     */
    QString birthDate() const;

    /*!
     * \property Player::federation
     * \brief the federation of the player
     *
     * This property holds the federation of the player.
     */
    QString federation() const;

    /*!
     * \property Player::origin
     * \brief the origin of the player
     *
     * This property holds the origin the player.
     */
    QString origin() const;

    /*!
     * \property Player::sex
     * \brief the sex of the player
     *
     * This property holds the sex of the player.
     */
    QString sex() const;

    Q_INVOKABLE QString titleString() const
    {
        return Player::titleString(m_title);
    };

    /*!
     * Returns the full name of the player.
     */
    QString fullName() const;

    /*!
     * Returns a JSON representation of the player.
     *
     * \sa fromJson()
     */
    QJsonObject toJson() const;

    /*!
     * Creates a player from its JSON representation.
     *
     * \a json The JSON representation of the player.
     *
     * \sa toJson()
     */
    static std::unique_ptr<Player> fromJson(const QJsonObject &json);

    /*!
     * Returns the TRF player field for this player.
     *
     * \a points The points of the player in the tournament.
     *
     * \a rank The rank of the player in the tournament.
     *
     * \a normalize Whether normalize fields to use latin characters only.
     */
    std::string toTrf(double points, int rank, bool normalize = true);

    friend QDebug operator<<(QDebug dbg, const Player &player);

public Q_SLOTS:
    void setId(int id);
    void setStartingRank(int startingRank);
    void setTitle(Player::Title title);
    void setTitle(const QString &titleString);
    void setName(const QString &name);
    void setSurname(const QString &surname);
    void setRating(int rating);
    void setNationalRating(int nationalRating);
    void setPlayerId(const QString &playerId);
    void setBirthDate(const QString &birthDate);
    void setFederation(const QString &federation);
    void setOrigin(const QString &origin);
    void setSex(const QString &sex);

Q_SIGNALS:
    void idChanged();
    void startingRankChanged();
    void titleChanged();
    void nameChanged();
    void surnameChanged();
    void ratingChanged();
    void nationalRatingChanged();
    void playerIdChanged();
    void birthDateChanged();
    void federationChanged();
    void originChanged();
    void sexChanged();

private:
    int m_id = 0;
    int m_startingRank = 1;
    Player::Title m_title;
    QString m_name;
    QString m_surname;
    int m_rating = 0;
    int m_nationalRating = 0;
    QString m_playerId;
    QString m_birthDate;
    QString m_federation;
    QString m_origin;
    QString m_sex;
};
