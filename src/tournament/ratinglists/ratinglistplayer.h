// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QObject>
#include <qqmlregistration.h>

using namespace Qt::StringLiterals;

struct RatingListPlayer {
    Q_GADGET

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString federation READ federation CONSTANT)
    Q_PROPERTY(QString gender READ gender CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString birthDate READ birthDate CONSTANT)
    Q_PROPERTY(int standardRating READ standardRating CONSTANT)
    Q_PROPERTY(int rapidRating READ rapidRating CONSTANT)
    Q_PROPERTY(int blitzRating READ blitzRating CONSTANT)
    Q_PROPERTY(QString nationalId READ nationalId CONSTANT)
    Q_PROPERTY(int nationalRating READ nationalRating CONSTANT)
    Q_PROPERTY(QJsonObject extra READ extra CONSTANT)

    Q_PROPERTY(QString origin READ origin CONSTANT)

public:
    explicit RatingListPlayer() = default;

    explicit RatingListPlayer(QString id,
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
                              QJsonObject extra);

    [[nodiscard]] QString id() const;

    [[nodiscard]] QString name() const;

    [[nodiscard]] QString federation() const;

    [[nodiscard]] QString gender() const;

    [[nodiscard]] QString title() const;

    [[nodiscard]] QString birthDate() const;

    [[nodiscard]] int standardRating() const;

    [[nodiscard]] int rapidRating() const;

    [[nodiscard]] int blitzRating() const;

    [[nodiscard]] QString nationalId() const;

    [[nodiscard]] int nationalRating() const;

    [[nodiscard]] QString origin() const;

    QJsonObject &extra();

    [[nodiscard]] QByteArray extraString() const;

    void setId(const QString &id);

    void setName(const QString &name);

    void setGender(const QString &gender);

    void setBirthDate(const QString &birthDate);

    void setStandardRating(int standardRating);

    void setNationalId(const QString &nationalId);

    void setNationalRating(int nationalRating);

private:
    QString m_id;
    QString m_name;
    QString m_federation;
    QString m_gender;
    QString m_title;
    QString m_birthDate;
    int m_standardRating{};
    int m_rapidRating{};
    int m_blitzRating{};
    QString m_nationalId;
    int m_nationalRating{};
    QJsonObject m_extra;
};
