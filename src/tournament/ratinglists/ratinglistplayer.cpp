// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglistplayer.h"

RatingListPlayer::RatingListPlayer(QString id,
                                   QString name,
                                   QString federation,
                                   QString gender,
                                   QString title,
                                   QString birthDate,
                                   int standardRating,
                                   int rapidRating,
                                   int blitzRating,
                                   QString nationalId,
                                   int nationalRating,
                                   QJsonObject extra)
    : m_id(std::move(id))
    , m_name(std::move(name))
    , m_federation(std::move(federation))
    , m_gender(std::move(gender))
    , m_title(std::move(title))
    , m_birthDate(std::move(birthDate))
    , m_standardRating(standardRating)
    , m_rapidRating(rapidRating)
    , m_blitzRating(blitzRating)
    , m_nationalId(std::move(nationalId))
    , m_nationalRating(nationalRating)
    , m_extra(std::move(extra))
{
}

QString RatingListPlayer::id() const
{
    return m_id;
}

void RatingListPlayer::setId(const QString &id)
{
    m_id = id;
}

QString RatingListPlayer::name() const
{
    return m_name;
}

void RatingListPlayer::setName(const QString &name)
{
    m_name = name;
}

QString RatingListPlayer::federation() const
{
    return m_federation;
}

QString RatingListPlayer::gender() const
{
    return m_gender;
}

void RatingListPlayer::setGender(const QString &gender)
{
    m_gender = gender;
}

QString RatingListPlayer::title() const
{
    return m_title;
}

QString RatingListPlayer::birthDate() const
{
    return m_birthDate;
}

void RatingListPlayer::setBirthDate(const QString &birthDate)
{
    m_birthDate = birthDate;
}

int RatingListPlayer::standardRating() const
{
    return m_standardRating;
}

void RatingListPlayer::setStandardRating(int standardRating)
{
    m_standardRating = standardRating;
}

int RatingListPlayer::rapidRating() const
{
    return m_rapidRating;
}

int RatingListPlayer::blitzRating() const
{
    return m_blitzRating;
}

QString RatingListPlayer::nationalId() const
{
    return m_nationalId;
}

void RatingListPlayer::setNationalId(const QString &nationalId)
{
    m_nationalId = nationalId;
}

int RatingListPlayer::nationalRating() const
{
    return m_nationalRating;
}

void RatingListPlayer::setNationalRating(int nationalRating)
{
    m_nationalRating = nationalRating;
}

QString RatingListPlayer::origin() const
{
    return m_extra.value(u"origin"_s).toString();
}

QJsonObject &RatingListPlayer::extra()
{
    return m_extra;
}

QByteArray RatingListPlayer::extraString() const
{
    const auto doc = QJsonDocument{m_extra};
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
}

#include "moc_ratinglistplayer.cpp"
