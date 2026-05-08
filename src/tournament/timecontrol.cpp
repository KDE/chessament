// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timecontrol.h"
#include "utils.h"

#include <QJsonArray>

using namespace Qt::StringLiterals;

TimeControlPeriod::TimeControlPeriod(std::optional<int> moves, int time, int increment)
    : m_moves(moves)
    , m_time(time)
    , m_increment(increment)
{
}

std::optional<int> TimeControlPeriod::moves() const
{
    return m_moves;
}

int TimeControlPeriod::time() const
{
    return m_time;
}

int TimeControlPeriod::increment() const
{
    return m_increment;
}

QJsonObject TimeControlPeriod::toJson() const
{
    auto result = QJsonObject{{
        {u"time"_s, m_time},
        {u"increment"_s, m_increment},
    }};

    if (m_moves) {
        result[u"moves"_s] = m_moves.value();
    }

    return result;
}

QString TimeControlPeriod::toTrf() const
{
    QString result;

    if (m_moves) {
        result += QString::number(m_moves.value()) % u'/';
    }

    result += QString::number(m_time);

    if (m_increment != 0) {
        result += u'+' % QString::number(m_increment);
    }

    return result;
}

TimeControlPeriod TimeControlPeriod::fromJson(QJsonObject json)
{
    TimeControlPeriod period{};

    if (const auto value = json["moves"_L1]; value.isDouble()) {
        period.m_moves = value.toInt();
    }
    if (const auto value = json["time"_L1]; value.isDouble()) {
        period.m_time = value.toInt();
    }
    if (const auto value = json["increment"_L1]; value.isDouble()) {
        period.m_increment = value.toInt();
    }

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
    periods.erase(periods.begin() + index);

    value = periods;
}

QJsonObject TimeControl::json()
{
    return m_json;
}

QString TimeControl::toTrf() const
{
    QStringList values;

    for (const auto period : periods()) {
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
