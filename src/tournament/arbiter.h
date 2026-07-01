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

    Arbiter::Role role();
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString arbiterId() const;
    [[nodiscard]] QByteArray extraString() const;

    [[nodiscard]] QJsonObject toJson() const;
    [[nodiscard]] QString toTrf() const;

    static std::unique_ptr<Arbiter> fromJson(const QJsonObject &obj);
    static std::unique_ptr<Arbiter> fromTrf(const QString &text);

public Q_SLOTS:
    void setRole(Arbiter::Role role);
    void setTitle(const QString &title);
    void setName(const QString &name);
    void setArbiterId(const QString &arbiterId);
    void setExtra(const QByteArray &extra);

Q_SIGNALS:
    void titleChanged();
    void nameChanged();
    void arbiterIdChanged();

private:
    Arbiter::Role m_role{};
    QString m_title;
    QString m_name;
    QString m_id;
    QJsonObject m_extra;
};
