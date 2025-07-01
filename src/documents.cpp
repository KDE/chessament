// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.h"
#include "document.h"

#include <KLocalizedString>

std::unique_ptr<Document> Controller::playersDocument()
{
    using enum PlayersModel::Columns;

    auto doc = std::make_unique<Document>();

    doc->addTitle(1, m_tournament->name());
    doc->addTitle(2, i18nc("@title", "Starting Rank"));

    PlayersModel model;
    model.setPlayers(m_tournament->players());
    model.setColumns({StartingRank, Title, FullName, Rating, NationalRating, PlayerId, Federation, Origin});
    doc->addTable(model);

    return doc;
}
void Controller::savePlayersDocument(const QString &fileName)
{
    auto doc = playersDocument();

    doc->saveAs(fileName);
}

void Controller::printPlayersDocument()
{
    auto doc = playersDocument();

    doc->print();
}
