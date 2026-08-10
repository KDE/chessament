// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QMimeType>
#include <QUrl>
#include <qqmlregistration.h>

class RatingList : public QObject
{
    Q_OBJECT

public:
    explicit RatingList() = default;

    explicit RatingList(QString name);

    [[nodiscard]] int id() const;

    [[nodiscard]] QString name() const;

    [[nodiscard]] QString url() const;

    [[nodiscard]] QString etag() const;

    [[nodiscard]] QString lastModified() const;

    [[nodiscard]] QByteArray extraString() const;

    static bool isSupportedUrl(const QString &location);

public Q_SLOTS:
    void setId(int id);

    void setName(const QString &name);

    void setUrl(const QString &url);

    void setEtag(const QString &etag);

    void setLastModified(const QString &lastModified);

    void setExtra(const QByteArray &extra);

private:
    int m_id{};
    QString m_name;
    QString m_url;
    QString m_etag;
    QString m_lastModified;
    QJsonObject m_extra;
};
