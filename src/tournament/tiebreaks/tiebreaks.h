// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "tiebreak.h"

#include <QJsonObject>

struct Tiebreaks {
    [[nodiscard]] int size() const;

    [[nodiscard]] std::vector<std::unique_ptr<Tiebreak>> all() const;

    [[nodiscard]] std::unique_ptr<Tiebreak> at(int index) const;

    void addTiebreak(std::unique_ptr<Tiebreak> arbiter);

    void setTiebreak(int index, std::unique_ptr<Tiebreak> arbiter);

    void swapTiebreaks(int a, int b);

    void removeTiebreak(int index);

    [[nodiscard]] QJsonObject toJson() const;

    static Tiebreaks fromJson(const QJsonObject &json);

    static std::unique_ptr<Tiebreak> tiebreak(const QString &id);

    static std::unique_ptr<Tiebreak> tiebreak(const QJsonObject &json);

    static std::expected<std::unique_ptr<Tiebreak>, QString> tiebreakFromTrf(const QString &code);

private:
    QJsonObject m_json;
};
