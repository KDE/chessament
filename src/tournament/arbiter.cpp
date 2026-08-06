// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "arbiter.h"

#include <KLocalizedString>
#include <QJsonArray>

#include "utils.h"

using namespace Qt::StringLiterals;

Arbiter::Arbiter(const QString &name)
{
    setName(name);
}

Arbiter::Role Arbiter::role() const
{
    return Arbiter::Role(m_json["role"_L1].toInt());
}

void Arbiter::setRole(Arbiter::Role role)
{
    if (this->role() == role) {
        return;
    }
    m_json["role"_L1] = std::to_underlying(role);
    Q_EMIT roleChanged();
}

QString Arbiter::title() const
{
    return m_json["title"_L1].toString();
}

void Arbiter::setTitle(const QString &title)
{
    if (this->title() == title) {
        return;
    }
    m_json["title"_L1] = title;
    Q_EMIT titleChanged();
}

QString Arbiter::name() const
{
    return m_json["name"_L1].toString();
}

void Arbiter::setName(const QString &name)
{
    if (this->name() == name) {
        return;
    }
    m_json["name"_L1] = name;
    Q_EMIT nameChanged();
}

QString Arbiter::arbiterId() const
{
    return m_json["id"_L1].toString();
}

void Arbiter::setArbiterId(const QString &arbiterId)
{
    if (this->arbiterId() == arbiterId) {
        return;
    }
    m_json["id"_L1] = arbiterId;
    Q_EMIT arbiterIdChanged();
}

QJsonObject Arbiter::toJson() const
{
    return m_json;
}

QString Arbiter::toTrf() const
{
    QString result;

    const auto arbiterTitle = title();
    if (!arbiterTitle.isEmpty()) {
        result += arbiterTitle % u' ';
    }

    result.append(name());

    const auto id = arbiterId();
    if (!id.isEmpty()) {
        result += " ("_L1 % id % u')';
    }

    return result;
}

std::unique_ptr<Arbiter> Arbiter::fromJson(const QJsonObject &obj)
{
    auto arbiter = std::make_unique<Arbiter>();
    arbiter->m_json = obj;
    return arbiter;
}

std::unique_ptr<Arbiter> Arbiter::fromTrf(const QString &text)
{
    auto arbiter = std::make_unique<Arbiter>();

    static const QRegularExpression re{R"(^([A-Z]{2} )?([^\(\n]+)(\((\d+)\))?$)"_L1};

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

int Arbiters::size() const
{
    const auto value = m_json["arbiters"_L1];
    if (!value.isArray()) {
        return 0;
    }

    return static_cast<int>(value.toArray().size());
}

std::vector<std::unique_ptr<Arbiter>> Arbiters::all() const
{
    std::vector<std::unique_ptr<Arbiter>> result;

    const auto arbiters = m_json["arbiters"_L1].toArray();
    result.reserve(arbiters.size());

    for (const auto arbiter : arbiters) {
        result.push_back(Arbiter::fromJson(arbiter.toObject()));
    }

    return result;
}

std::unique_ptr<Arbiter> Arbiters::at(int index) const
{
    Q_ASSERT(index >= 0);

    const auto arbiters = m_json["arbiters"_L1].toArray();
    Q_ASSERT(index < arbiters.size());

    return Arbiter::fromJson(arbiters[index].toObject());
}

void Arbiters::addArbiter(std::unique_ptr<Arbiter> arbiter)
{
    auto value = m_json["arbiters"_L1];
    QJsonArray arbiters;

    if (value.isArray()) {
        arbiters = value.toArray();
    }

    arbiters << arbiter->toJson();
    value = arbiters;
}

void Arbiters::setArbiter(int index, std::unique_ptr<Arbiter> arbiter)
{
    Q_ASSERT(index >= 0);

    auto value = m_json["arbiters"_L1];
    Q_ASSERT(value.isArray());

    auto arbiters = value.toArray();
    Q_ASSERT(index < arbiters.size());

    auto json = arbiters[index].toObject();
    Utils::updateObject(&json, arbiter->toJson());
    arbiters[index] = json;

    value = arbiters;
}

void Arbiters::removeArbiter(int index)
{
    Q_ASSERT(index >= 0);

    auto value = m_json["arbiters"_L1];
    Q_ASSERT(value.isArray());

    auto arbiters = value.toArray();
    Q_ASSERT(index < arbiters.size());

    arbiters.erase(arbiters.begin() + index);

    value = arbiters;
}

QJsonObject Arbiters::toJson() const
{
    return m_json;
}

Arbiters Arbiters::fromJson(const QJsonObject &json)
{
    Arbiters arbiters;
    arbiters.m_json = json;
    return arbiters;
}

#include "moc_arbiter.cpp"
