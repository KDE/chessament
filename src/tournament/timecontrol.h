// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>

#include <optional>
#include <vector>

struct TimeControlPeriod {
    explicit TimeControlPeriod() = default;

    explicit TimeControlPeriod(std::optional<int> moves, int time, int increment);

    [[nodiscard]] std::optional<int> moves() const;

    [[nodiscard]] int time() const;

    [[nodiscard]] int increment() const;

    [[nodiscard]] QJsonObject toJson() const;

    [[nodiscard]] QString toTrf() const;

    static TimeControlPeriod fromJson(QJsonObject json);

private:
    std::optional<int> m_moves{std::nullopt};
    int m_time{};
    int m_increment{};
};

struct TimeControl {
    enum class Format {
        Blitz,
        Rapid,
        Classical,
    };

    explicit TimeControl() = default;

    explicit TimeControl(std::initializer_list<TimeControlPeriod> periods);

    [[nodiscard]] std::vector<TimeControlPeriod> periods() const;

    void addPeriod();

    void addPeriod(const TimeControlPeriod &period);

    void setPeriod(int index, const TimeControlPeriod &period);

    void removePeriod(int index);

    [[nodiscard]] std::chrono::seconds durationPerPlayer() const;

    [[nodiscard]] TimeControl::Format format() const;

    QJsonObject json();

    [[nodiscard]] QString toTrf() const;

    bool operator==(const TimeControl &other) const;

    static TimeControl fromJson(QJsonObject json);

    static TimeControl fromTrf(const QString &value);

private:
    QJsonObject m_json;
};
