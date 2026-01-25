// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QString>

using namespace Qt::StringLiterals;

/*!
 * \class Player
 * \inmodule tournament
 * \inheaderfile tournament/player.h
 */
class Player : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(int startingRank READ startingRank NOTIFY startingRankChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
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

    static uint titleStrengthLevel(const QString &title)
    {
        const auto t = title.toLower();
        if (t == QStringLiteral("gm")) {
            return 0;
        }
        if (t == QStringLiteral("im")) {
            return 1;
        }
        if (t == QStringLiteral("wgm")) {
            return 2;
        }
        if (t == QStringLiteral("fm")) {
            return 3;
        }
        if (t == QStringLiteral("wim")) {
            return 4;
        }
        if (t == QStringLiteral("cm")) {
            return 5;
        }
        if (t == QStringLiteral("wfm")) {
            return 6;
        }
        if (t == QStringLiteral("wcm")) {
            return 7;
        }
        return 8;
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
        const auto t = title.toLower();
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
                    const QString &title,
                    const QString &name,
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
    [[nodiscard]] int id() const;

    /*!
     * \property Player::startingRank
     * \brief the ID of the player
     *
     * This property holds the starting rank of the player.
     */
    [[nodiscard]] int startingRank() const;

    /*!
     * \property Player::title
     * \brief the title of the player
     *
     * This property holds the title of the player.
     */
    [[nodiscard]] QString title() const;

    /*!
     * \property Player::name
     * \brief the name of the player
     *
     * This property holds the name of the player.
     */
    [[nodiscard]] QString name() const;

    /*!
     * \property Player::rating
     * \brief the rating of the player
     *
     * This property holds the rating of the player.
     */
    [[nodiscard]] int rating() const;

    /*!
     * \property Player::nationalRating
     * \brief the national rating of the player
     *
     * This property holds the national rating of the player.
     */
    [[nodiscard]] int nationalRating() const;

    /*!
     * \property Player::playerId
     * \brief the player ID of the player
     *
     * This property holds the player ID of the player.
     */
    [[nodiscard]] QString playerId() const;

    /*!
     * \property Player::birthDate
     * \brief the birth date of the player
     *
     * This property holds the birth date of the player.
     */
    [[nodiscard]] QString birthDate() const;

    /*!
     * \property Player::federation
     * \brief the federation of the player
     *
     * This property holds the federation of the player.
     */
    [[nodiscard]] QString federation() const;

    /*!
     * \property Player::origin
     * \brief the origin of the player
     *
     * This property holds the origin the player.
     */
    [[nodiscard]] QString origin() const;

    /*!
     * \property Player::sex
     * \brief the sex of the player
     *
     * This property holds the sex of the player.
     */
    [[nodiscard]] QString sex() const;

    [[nodiscard]] QByteArray extraString() const;

    /*!
     * Returns a JSON representation of the player.
     *
     * \sa fromJson()
     */
    [[nodiscard]] QJsonObject toJson() const;

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

    static std::function<bool(const Player *a, const Player *b)> SortByStartingRank;

    friend QDebug operator<<(QDebug dbg, const Player &player);

public Q_SLOTS:
    void setId(int id);
    void setStartingRank(int startingRank);
    void setTitle(const QString &titleString);
    void setName(const QString &name);
    void setRating(int rating);
    void setNationalRating(int nationalRating);
    void setPlayerId(const QString &playerId);
    void setBirthDate(const QString &birthDate);
    void setFederation(const QString &federation);
    void setOrigin(const QString &origin);
    void setSex(const QString &sex);
    void setExtra(const QByteArray &extra);

Q_SIGNALS:
    void idChanged();
    void startingRankChanged();
    void titleChanged();
    void nameChanged();
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
    QString m_title;
    QString m_name;
    int m_rating = 0;
    int m_nationalRating = 0;
    QString m_playerId;
    QString m_birthDate;
    QString m_federation;
    QString m_origin;
    QString m_sex;
    QJsonObject m_extra;
};
