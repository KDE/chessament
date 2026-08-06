// SPDX-FileCopyrightText: 2025-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tiebreaks.h"

#include <QJsonArray>

#include "tiebreaks/aob.h"
#include "tiebreaks/buchholz.h"
#include "tiebreaks/dummy.h"
#include "tiebreaks/numberwins.h"
#include "tiebreaks/playedblack.h"
#include "tiebreaks/points.h"
#include "tiebreaks/won.h"
#include "utils.h"

using namespace Qt::Literals::StringLiterals;

int Tiebreaks::size() const
{
    const auto value = m_json["tiebreaks"_L1];
    if (!value.isArray()) {
        return 0;
    }

    return static_cast<int>(value.toArray().size());
}

std::vector<std::unique_ptr<Tiebreak>> Tiebreaks::all() const
{
    std::vector<std::unique_ptr<Tiebreak>> result;

    const auto tibreaks = m_json["tiebreaks"_L1].toArray();
    result.reserve(tibreaks.size());

    for (const auto value : tibreaks) {
        const auto options = value.toObject();
        auto tiebreak = Tiebreaks::tiebreak(options);

        result.push_back(std::move(tiebreak));
    }

    return result;
}

std::unique_ptr<Tiebreak> Tiebreaks::at(int index) const
{
    Q_ASSERT(index >= 0);

    const auto tiebreaks = m_json["tiebreaks"_L1].toArray();
    Q_ASSERT(index < tiebreaks.size());

    const auto tiebreakJson = tiebreaks.at(index);
    const auto options = tiebreakJson.toObject();
    auto tiebreak = Tiebreaks::tiebreak(options);

    return tiebreak;
}

void Tiebreaks::addTiebreak(std::unique_ptr<Tiebreak> arbiter)
{
    auto value = m_json["tiebreaks"_L1];
    QJsonArray tiebreaks;

    if (value.isArray()) {
        tiebreaks = value.toArray();
    }

    tiebreaks << arbiter->toJson();
    value = tiebreaks;
}

void Tiebreaks::setTiebreak(int index, std::unique_ptr<Tiebreak> arbiter)
{
    Q_ASSERT(index >= 0);

    auto value = m_json["tiebreaks"_L1];
    Q_ASSERT(value.isArray());

    auto tiebreaks = value.toArray();
    Q_ASSERT(index < tiebreaks.size());

    auto json = tiebreaks[index].toObject();
    Utils::updateObject(&json, arbiter->toJson());
    tiebreaks[index] = json;

    value = tiebreaks;
}

void Tiebreaks::swapTiebreaks(int a, int b)
{
    Q_ASSERT(a >= 0);
    Q_ASSERT(b >= 0);

    auto value = m_json["tiebreaks"_L1];
    Q_ASSERT(value.isArray());

    auto tiebreaks = value.toArray();
    Q_ASSERT(a < tiebreaks.size());
    Q_ASSERT(b < tiebreaks.size());

    auto tiebreak = tiebreaks.takeAt(a);
    tiebreaks.insert(b, tiebreak);

    value = tiebreaks;
}

void Tiebreaks::removeTiebreak(int index)
{
    Q_ASSERT(index >= 0);

    auto value = m_json["tiebreaks"_L1];
    Q_ASSERT(value.isArray());

    auto tiebreaks = value.toArray();
    Q_ASSERT(index < tiebreaks.size());

    tiebreaks.erase(tiebreaks.begin() + index);

    value = tiebreaks;
}

QJsonObject Tiebreaks::toJson() const
{
    return m_json;
}

Tiebreaks Tiebreaks::fromJson(const QJsonObject &json)
{
    Tiebreaks tiebreaks;
    tiebreaks.m_json = json;
    return tiebreaks;
}

std::unique_ptr<Tiebreak> Tiebreaks::tiebreak(const QString &id)
{
    if (id == "pts"_L1) {
        return std::make_unique<Points>();
    }
    if (id == "bh"_L1) {
        return std::make_unique<Buchholz>();
    }
    if (id == "win"_L1) {
        return std::make_unique<NumberOfWins>();
    }
    if (id == "won"_L1) {
        return std::make_unique<NumberOfGamesWon>();
    }
    if (id == "bpg"_L1) {
        return std::make_unique<NumberOfGamesPlayedWithBlack>();
    }
    if (id == "aob"_L1) {
        return std::make_unique<AverageBuchholzOfOpponents>();
    }
    return nullptr;
}

std::unique_ptr<Tiebreak> Tiebreaks::tiebreak(const QJsonObject &json)
{
    const auto id = json["id"_L1].toString();

    auto tiebreak = Tiebreaks::tiebreak(id);

    if (tiebreak == nullptr) {
        tiebreak = std::make_unique<DummyTiebreak>();
    }

    tiebreak->setOptions(json.toVariantMap());

    return tiebreak;
}

std::expected<std::unique_ptr<Tiebreak>, QString> Tiebreaks::tiebreakFromTrf(const QString &code)
{
    if (code.startsWith("OTHER_"_L1, Qt::CaseSensitivity::CaseInsensitive)) {
        qWarning() << "Unsupported tiebreak" << code;
        return nullptr;
    }

    const auto options = code.split(u'/', Qt::SkipEmptyParts);

    auto tiebreak = Tiebreaks::tiebreak(options[0].toLower());
    if (tiebreak == nullptr) {
        qWarning() << "Unsupported tiebreak" << code;
        return nullptr;
    }

    if (const auto ok = tiebreak->setTrfOptions(options.mid(1)); !ok) {
        return std::unexpected(ok.error());
    }

    return tiebreak;
}
