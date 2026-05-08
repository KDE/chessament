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

    static TimeControlPeriod fromJson(QJsonObject json);

private:
    std::optional<int> m_moves{std::nullopt};
    int m_time{};
    int m_increment{};
};

struct TimeControl {
    [[nodiscard]] std::vector<TimeControlPeriod> periods() const;

    void addPeriod();

    void setPeriod(int index, const TimeControlPeriod &period);

    void removePeriod(int index);

    QJsonObject json();

    static TimeControl fromJson(QJsonObject json);

private:
    QJsonObject m_json;
};
