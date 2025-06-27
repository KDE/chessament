// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.h"
#include "document.h"

#include <QPainter>
#include <QPdfDocument>
#include <QPrintDialog>
#include <QPrinter>

#include <KLocalizedString>

QString Controller::getStartingRankDocument()
{
    using enum PlayersModel::Columns;

    Document doc;

    doc.addTitle(1, m_tournament->name());
    doc.addTitle(2, i18nc("@title", "Starting Rank"));

    PlayersModel model;
    model.setPlayers(m_tournament->players());
    model.setColumns({StartingRank, Title, FullName, Rating, NationalRating, PlayerId, Federation, Origin});
    doc.addTable(model);

    return doc.toHTML();
}

void Controller::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageMargins({0, 0, 0, 0});
    printer.setFullPage(true);

    QPrintDialog dialog(&printer);

    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter;
        painter.begin(&printer);

        QPdfDocument pdf;
        pdf.load(m_tempfile->fileName());

        for (int i = 0; i < pdf.pageCount(); i++) {
            auto size = printer.pageRect(QPrinter::DevicePixel).size().toSize();
            auto image = pdf.render(i, size);
            painter.drawImage(0, 0, image);

            if (i != pdf.pageCount() - 1) {
                printer.newPage();
            }
        }

        painter.end();
    }
}
