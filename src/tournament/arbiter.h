// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QObject>
#include <qqmlregistration.h>

class Arbiter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(Arbiter::Role role READ role WRITE setRole NOTIFY roleChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString arbiterId READ arbiterId WRITE setArbiterId NOTIFY arbiterIdChanged)

public:
    enum class Role {
        Arbiter,
        Chief,
        Deputy,
    };
    Q_ENUM(Role);

    explicit Arbiter() = default;

    explicit Arbiter(const QString &name);

    [[nodiscard]] Arbiter::Role role() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString arbiterId() const;

    [[nodiscard]] QJsonObject toJson() const;
    [[nodiscard]] QString toTrf() const;

    static std::unique_ptr<Arbiter> fromJson(const QJsonObject &obj);
    static std::unique_ptr<Arbiter> fromTrf(const QString &text);

public Q_SLOTS:
    void setRole(Arbiter::Role role);
    void setTitle(const QString &title);
    void setName(const QString &name);
    void setArbiterId(const QString &arbiterId);

Q_SIGNALS:
    void roleChanged();
    void titleChanged();
    void nameChanged();
    void arbiterIdChanged();

private:
    QJsonObject m_json;
};

struct Arbiters {
    [[nodiscard]] int size() const;

    [[nodiscard]] std::vector<std::unique_ptr<Arbiter>> all() const;

    [[nodiscard]] std::unique_ptr<Arbiter> at(int index) const;

    void addArbiter(std::unique_ptr<Arbiter> arbiter);

    void setArbiter(int index, std::unique_ptr<Arbiter> arbiter);

    void removeArbiter(int index);

    [[nodiscard]] QJsonObject toJson() const;

    static Arbiters fromJson(const QJsonObject &json);

private:
    QJsonObject m_json;
};
