// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <AbstractKirigamiApplication>

using namespace Qt::StringLiterals;

class ChessamentApplication : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ChessamentApplication(QObject *parent = nullptr);

Q_SIGNALS:
    void newTournament();
    void openTournament();
    void saveTournamentAs();
    void importTrf();
    void exportTrf();

    void connectAccount();

private:
    void setupActions() override;
};
