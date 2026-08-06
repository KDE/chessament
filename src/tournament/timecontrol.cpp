// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timecontrol.h"
#include "utils.h"

#include <QJsonArray>
#include <chrono>

using namespace std::chrono_literals;
using namespace Qt::StringLiterals;

TimeControlPeriod::TimeControlPeriod()
    : m_json{
          {"time"_L1, 1},
          {"increment"_L1, 0},
      }
{
}

TimeControlPeriod::TimeControlPeriod(std::optional<int> moves, int time, int increment)
    : m_json{
          {"time"_L1, time},
          {"increment"_L1, increment},
      }
{
    if (moves) {
        m_json["moves"_L1] = moves.value();
    }
}

std::optional<int> TimeControlPeriod::moves() const
{
    if (m_json.contains("moves"_L1)) {
        return m_json["moves"_L1].toInt();
    }

    return std::nullopt;
}

int TimeControlPeriod::time() const
{
    return m_json["time"_L1].toInt();
}

int TimeControlPeriod::increment() const
{
    return m_json["increment"_L1].toInt();
}

QJsonObject TimeControlPeriod::toJson() const
{
    return m_json;
}

QString TimeControlPeriod::toTrf() const
{
    QString result;

    const auto periodMoves = moves();
    if (moves()) {
        result += QString::number(periodMoves.value()) % u'/';
    }

    result += QString::number(time());

    const auto periodIncrement = increment();
    if (periodIncrement != 0) {
        result += u'+' % QString::number(periodIncrement);
    }

    return result;
}

TimeControlPeriod TimeControlPeriod::fromJson(const QJsonObject &json)
{
    TimeControlPeriod period{};
    period.m_json = json;
    return period;
}

TimeControl::TimeControl(std::initializer_list<TimeControlPeriod> periods)
{
    for (const auto &period : periods) {
        addPeriod(period);
    }
}

std::vector<TimeControlPeriod> TimeControl::periods() const
{
    auto value = m_json["periods"_L1];
    if (!value.isArray()) {
        return {};
    }

    std::vector<TimeControlPeriod> result;

    for (const auto period : value.toArray()) {
        result.push_back(TimeControlPeriod::fromJson(period.toObject()));
    }

    return result;
}

void TimeControl::addPeriod()
{
    auto value = m_json["periods"_L1];
    QJsonArray periods;

    if (value.isArray()) {
        periods = value.toArray();
    }

    periods << TimeControlPeriod{}.toJson();
    value = periods;
}

void TimeControl::addPeriod(const TimeControlPeriod &period)
{
    auto value = m_json["periods"_L1];
    QJsonArray periods;

    if (value.isArray()) {
        periods = value.toArray();
    }

    periods << period.toJson();
    value = periods;
}

void TimeControl::setPeriod(int index, const TimeControlPeriod &period)
{
    auto value = m_json["periods"_L1];
    Q_ASSERT(value.isArray());

    auto periods = value.toArray();
    auto json = periods[index].toObject();
    Utils::updateObject(&json, period.toJson());
    periods[index] = json;

    value = periods;
}

void TimeControl::removePeriod(int index)
{
    auto value = m_json["periods"_L1];
    Q_ASSERT(value.isArray());

    auto periods = value.toArray();
    Q_ASSERT(periods.size() > 1);

    periods.erase(periods.begin() + index);

    auto lastPeriod = periods.last().toObject();
    lastPeriod.remove("moves"_L1);
    periods[periods.size() - 1] = lastPeriod;

    value = periods;
}

std::chrono::seconds TimeControl::durationPerPlayer() const
{
    std::chrono::seconds duration{};

    for (const auto &period : periods()) {
        duration += std::chrono::seconds(period.time() + (period.increment() * 60));
    }

    return duration;
}

TimeControl::Format TimeControl::format() const
{
    const auto duration = durationPerPlayer();

    if (duration <= 10min) {
        return TimeControl::Format::Blitz;
    }
    if (duration < 60min) {
        return TimeControl::Format::Rapid;
    }
    return TimeControl::Format::Classical;
}

QJsonObject TimeControl::toJson() const
{
    return m_json;
}

QString TimeControl::toTrf() const
{
    QStringList values;

    for (const auto &period : periods()) {
        values << period.toTrf();
    }

    return values.join(u':');
}

bool TimeControl::operator==(const TimeControl &other) const
{
    return m_json == other.m_json;
}

TimeControl TimeControl::fromJson(QJsonObject json)
{
    TimeControl result{};
    result.m_json = std::move(json);
    return result;
}

TimeControl TimeControl::fromTrf(const QString &value)
{
    TimeControl timeControl{};

    const auto periods = value.split(u':');

    for (const auto &period : periods) {
        static const QRegularExpression periodRegex{uR"(((\d+)\/)?(\d+)(\+(\d+))?)"_s};
        const auto match = periodRegex.match(period);

        bool ok{};
        std::optional<int> moves{};
        const auto m = match.captured(2).toInt(&ok);
        if (ok) {
            moves = m;
        }

        const auto time = match.captured(3).toInt();
        const auto increment = match.captured(5).toInt();

        timeControl.addPeriod(TimeControlPeriod{moves, time, increment});
    }

    return timeControl;
}
