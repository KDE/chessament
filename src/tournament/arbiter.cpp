// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "arbiter.h"

#include <KLocalizedString>
#include <QJsonArray>

using namespace Qt::StringLiterals;

Arbiter::Arbiter(const QString &name)
    : m_name(name)
{
}

Arbiter::Role Arbiter::role()
{
    return m_role;
}

void Arbiter::setRole(Arbiter::Role role)
{
    if (m_role == role) {
        return;
    }
    m_role = role;
}

QString Arbiter::title() const
{
    return m_title;
}

void Arbiter::setTitle(const QString &title)
{
    if (m_title == title) {
        return;
    }
    m_title = title;
    Q_EMIT titleChanged();
}

QString Arbiter::name() const
{
    return m_name;
}

void Arbiter::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }
    m_name = name;
    Q_EMIT nameChanged();
}

QString Arbiter::arbiterId() const
{
    return m_id;
}

void Arbiter::setArbiterId(const QString &arbiterId)
{
    if (m_id == arbiterId) {
        return;
    }
    m_id = arbiterId;
    Q_EMIT arbiterIdChanged();
}

QByteArray Arbiter::extraString() const
{
    const auto doc = QJsonDocument{m_extra};
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
}

void Arbiter::setExtra(const QByteArray &extra)
{
    const auto doc = QJsonDocument::fromJson(extra);

    Q_ASSERT(doc.isObject());

    m_extra = doc.object();
}

QJsonObject Arbiter::toJson() const
{
    return {
        {u"role"_s, std::to_underlying(m_role)},
        {u"title"_s, m_title},
        {u"name"_s, m_name},
        {u"id"_s, m_id},
    };
}

QString Arbiter::toTrf() const
{
    QString result;

    if (!m_title.isEmpty()) {
        result += m_title % u' ';
    }

    result.append(m_name);

    if (!m_id.isEmpty()) {
        result += " ("_L1 % m_id % u')';
    }

    return result;
}

std::unique_ptr<Arbiter> Arbiter::fromJson(const QJsonObject &obj)
{
    auto arbiter = std::make_unique<Arbiter>();
    arbiter->setRole(Arbiter::Role(obj.value("role"_L1).toInt()));
    arbiter->setName(obj.value("name"_L1).toString());
    arbiter->setTitle(obj.value("title"_L1).toString());
    arbiter->setArbiterId(obj.value("id"_L1).toString());

    return arbiter;
}

std::unique_ptr<Arbiter> Arbiter::fromTrf(const QString &text)
{
    auto arbiter = std::make_unique<Arbiter>();

    static QRegularExpression re{R"(^([A-Z]{2} )?([^\(\n]+)(\((\d+)\))?$)"_L1};

    const auto match = re.match(text);
    if (match.hasMatch()) {
        const QString title = match.captured(1).trimmed();
        const QString name = match.captured(2).trimmed();
        const QString arbiterId = match.captured(4);

        arbiter->setTitle(title);
        arbiter->setName(name);
        arbiter->setArbiterId(arbiterId);
    } else {
        arbiter->setName(text);
    }

    return arbiter;
}

#include "moc_arbiter.cpp"
