// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglist.h"

using namespace Qt::StringLiterals;

RatingList::RatingList(QString name)
    : m_name(std::move(name))
{
}

int RatingList::id() const
{
    return m_id;
}

void RatingList::setId(int id)
{
    m_id = id;
}

QString RatingList::name() const
{
    return m_name;
}

void RatingList::setName(const QString &name)
{
    m_name = name;
}

QString RatingList::url() const
{
    return m_url;
}

void RatingList::setUrl(const QString &url)
{
    m_url = url;
}

QString RatingList::lastModified() const
{
    return m_lastModified;
}

void RatingList::setLastModified(const QString &lastModified)
{
    m_lastModified = lastModified;
}

QJsonObject &RatingList::extra()
{
    return m_extra;
}

QByteArray RatingList::extraString() const
{
    const auto doc = QJsonDocument{m_extra};
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
}

void RatingList::setExtra(const QByteArray &extra)
{
    const auto doc = QJsonDocument::fromJson(extra);

    Q_ASSERT(doc.isObject());

    m_extra = doc.object();
}

bool RatingList::isSupportedUrl(const QString &location)
{
    const auto url = QUrl::fromUserInput(location);

    if (!url.isValid()) {
        return false;
    }
    if (url.scheme() != u"file"_s && url.scheme() != u"https"_s) {
        return false;
    }

    return true;
}

#include "moc_ratinglist.cpp"
