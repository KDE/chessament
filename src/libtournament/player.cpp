// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "player.h"
#include "utils.h"

#include <format>

using namespace Qt::Literals::StringLiterals;

Player::Player()
    : QObject()
{
}

Player::Player(int startingRank, const QString &name, int rating)
    : QObject()
{
    setStartingRank(startingRank);
    setName(name);
    setRating(rating);
}

Player::Player(int startingRank,
               Title title,
               const QString &name,
               const QString &surname,
               int rating,
               int nationalRating,
               const QString &playerId,
               const QString &birthDate,
               const QString &federation,
               const QString &origin,
               const QString &sex)
    : QObject()
{
    setStartingRank(startingRank);
    setTitle(title);
    setName(name);
    setSurname(surname);
    setRating(rating);
    setNationalRating(nationalRating);
    setPlayerId(playerId);
    setBirthDate(birthDate);
    setFederation(federation);
    setOrigin(origin);
    setSex(sex);
}

int Player::id() const
{
    return m_id;
}

void Player::setId(int id)
{
    if (m_id == id) {
        return;
    }
    m_id = id;
    Q_EMIT idChanged();
}

int Player::startingRank() const
{
    return m_startingRank;
}

void Player::setStartingRank(int startingRank)
{
    if (m_startingRank == startingRank) {
        return;
    }
    m_startingRank = startingRank;
    Q_EMIT startingRankChanged();
}

Player::Title Player::title() const
{
    return m_title;
}

void Player::setTitle(Title title)
{
    if (m_title == title) {
        return;
    }
    m_title = title;
    Q_EMIT titleChanged();
}

void Player::setTitle(const QString &titleString)
{
    auto title = titleForString(titleString);
    setTitle(title);
}

QString Player::name() const
{
    return m_name;
}

void Player::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }
    m_name = name;
    Q_EMIT nameChanged();
}

QString Player::surname() const
{
    return m_surname;
}

void Player::setSurname(const QString &surname)
{
    if (m_surname == surname) {
        return;
    }
    m_surname = surname;
    Q_EMIT surnameChanged();
}

int Player::rating() const
{
    return m_rating;
}

void Player::setRating(int rating)
{
    if (m_rating == rating) {
        return;
    }
    m_rating = rating;
    Q_EMIT ratingChanged();
}

int Player::nationalRating() const
{
    return m_nationalRating;
}

void Player::setNationalRating(int nationalRating)
{
    if (m_nationalRating == nationalRating) {
        return;
    }
    m_nationalRating = nationalRating;
    Q_EMIT nationalRatingChanged();
}

QString Player::playerId() const
{
    return m_playerId;
}

void Player::setPlayerId(const QString &playerId)
{
    if (m_playerId == playerId) {
        return;
    }
    m_playerId = playerId;
    Q_EMIT playerIdChanged();
}

QString Player::birthDate() const
{
    return m_birthDate;
}

void Player::setBirthDate(const QString &birthDate)
{
    if (m_birthDate == birthDate) {
        return;
    }
    m_birthDate = birthDate;
    Q_EMIT birthDateChanged();
}

QString Player::federation() const
{
    return m_federation;
}

void Player::setFederation(const QString &federation)
{
    if (m_federation == federation) {
        return;
    }
    m_federation = federation;
    Q_EMIT federationChanged();
}

QString Player::origin() const
{
    return m_origin;
}

void Player::setOrigin(const QString &origin)
{
    if (m_origin == origin) {
        return;
    }
    m_origin = origin;
    Q_EMIT originChanged();
}

QString Player::sex() const
{
    return m_sex;
}

void Player::setSex(const QString &sex)
{
    if (m_sex == sex) {
        return;
    }
    m_sex = sex;
    Q_EMIT sexChanged();
}

QString Player::fullName() const
{
    if (!m_surname.isEmpty()) {
        return m_surname + u", "_s + m_name;
    }
    return m_name;
}

QJsonObject Player::toJson() const
{
    QJsonObject json;

    json[u"starting_rank"_s] = m_startingRank;
    json[u"title"_s] = Player::titleString(m_title);
    json[u"name"_s] = m_name;
    json[u"surname"_s] = m_surname;
    json[u"rating"_s] = m_rating;
    json[u"national_rating"_s] = m_nationalRating;
    json[u"player_id"_s] = m_playerId;
    json[u"birth_date"_s] = m_birthDate;
    json[u"federation"_s] = m_federation;
    json[u"origin"_s] = m_origin;
    json[u"sex"_s] = m_sex;

    return json;
}

std::unique_ptr<Player> Player::fromJson(const QJsonObject &json)
{
    auto player = std::make_unique<Player>();

    if (const auto v = json[u"starting_rank"_s]; v.isDouble()) {
        player->m_startingRank = v.toInt();
    }
    if (const auto v = json[u"title"_s]; v.isString()) {
        player->m_title = Player::titleForString(v.toString());
    }
    if (const auto v = json[u"surname"_s]; v.isString()) {
        player->m_surname = v.toString();
    }
    if (const auto v = json[u"name"_s]; v.isString()) {
        player->m_name = v.toString();
    }
    if (const auto v = json[u"rating"_s]; v.isDouble()) {
        player->m_rating = v.toInt();
    }
    if (const auto v = json[u"national_rating"_s]; v.isDouble()) {
        player->m_nationalRating = v.toInt();
    }
    if (const auto v = json[u"player_id"_s]; v.isString()) {
        player->m_playerId = v.toString();
    }
    if (const auto v = json[u"birth_date"_s]; v.isString()) {
        player->m_birthDate = v.toString();
    }
    if (const auto v = json[u"federation"_s]; v.isString()) {
        player->m_federation = v.toString();
    }
    if (const auto v = json[u"origin"_s]; v.isString()) {
        player->m_origin = v.toString();
    }
    if (const auto v = json[u"sex"_s]; v.isString()) {
        player->m_sex = v.toString();
    }

    return player;
}

std::string Player::toTrf(double points, int rank, bool normalize)
{
    const auto title = Player::titleString(m_title);
    auto name = fullName();
    auto federation = m_federation;
    auto birth = m_birthDate;
    auto playerid = m_playerId;
    auto sex = m_sex;

    if (normalize) {
        sex = Utils::normalize(sex);
        name = Utils::normalize(name);
        federation = Utils::normalize(federation);
        playerid = Utils::normalize(playerid);
        birth = Utils::normalize(birth);
    }

    return std::format("001 {:4} {:1.1}{:3} {:33.33} {:4} {:3.3} {:>11} {:10.10} {:4.1f} {:4}",
                       m_startingRank,
                       sex.toStdString(),
                       title.toStdString(),
                       name.toStdString(),
                       m_rating,
                       federation.toStdString(),
                       playerid.toStdString(),
                       birth.toStdString(),
                       points,
                       rank)
        .c_str();
}

QDebug operator<<(QDebug dbg, const Player &player)
{
    dbg.nospace() << "Player(" << player.fullName() << ")";
    return dbg;
}
