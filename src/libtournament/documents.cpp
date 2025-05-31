// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tournament.h"

QString Tournament::getPlayersListDocument()
{
    QString html = QStringLiteral(
        "<!doctype html>"
        "<html>"
        "<head>"
        "<style>"
        "body {"
        "  background-color: #ffffff;"
        "}"
        ".table {"
        "  width: 100%;"
        "  margin-bottom: 20px;"
        "  border: 1px solid #dddddd;"
        "  border-collapse: collapse;"
        "}"
        ".table th {"
        "  font-weight: bold;"
        "  padding: 5px;"
        "  background: #efefef;"
        "  border: 1px solid #dddddd;"
        "}"
        ".table td {"
        "  border: 1px solid #dddddd;"
        "  padding: 5px;"
        "}"
        "</style>"
        "</head>"
        "<body>"
        "%1"
        "</body>"
        "</html>");
    QString body;

    body += QStringLiteral("<h1>%1</h1>").arg(m_name);

    QString table = QStringLiteral(
        "<table class=\"table\" width=\"100%\">"
        "<thead>"
        "%1"
        "</thead>"
        "<tbody>"
        "%2"
        "</tbody>"
        "</table>");

    QString rows;

    for (const auto &player : *m_players) {
        rows += QStringLiteral(
                    "<tr>"
                    "<td>%1</td>"
                    "<td>%2</td>"
                    "<td>%3</td>"
                    "</tr>")
                    .arg(player->startingRank())
                    .arg(player->name())
                    .arg(player->rating());
    }

    body += table.arg(QStringLiteral("<tr>"
                                     "<th>No.</th>"
                                     "<th>Name</th>"
                                     "<th>Rating</th>"
                                     "</tr>"),
                      rows);

    return html.arg(body);
}
