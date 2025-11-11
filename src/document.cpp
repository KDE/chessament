// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "document.h"

#include <QAbstractItemModel>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextFrame>

QT_BEGIN_NAMESPACE
Q_GUI_EXPORT extern int qt_defaultDpiX();
Q_GUI_EXPORT extern int qt_defaultDpiY();
QT_END_NAMESPACE

using namespace Qt::StringLiterals;

const QString DOCUMENT_TEMPLATE =
    uR"(
<!doctype html>
<html>
<body>
  %1
</body>
</html>
)"_s;

Document::Document()
    : m_doc(std::make_unique<QTextDocument>())
{
}

void Document::addTitle(uint level, const QString &text)
{
    Q_ASSERT(level >= 1 && level <= 2);

    uint fontSize;

    switch (level) {
    case 1:
        fontSize = 15;
        break;
    case 2:
        fontSize = 13;
        break;
    default:
        fontSize = 10;
    }

    m_content += u"<p style=\"font-size: %1pt; font-weight: bold; margin-top: 16px;\">%2</p>\n"_s.arg(QString::number(fontSize), text);
}

void Document::addTable(const QAbstractItemModel &model)
{
    m_content += u"<table cellpadding=\"0\" style=\"border-collapse: collapse; border: 1px solid #aaa; margin-top: 16px;\">\n"_s;

    m_content += u"<thead><tr bgcolor=\"#efefef\">\n"_s;
    for (auto i = 0; i < model.columnCount(); i++) {
        m_content += u"<th style=\"padding: 2px;\">%1</th>\n"_s.arg(model.headerData(i, Qt::Horizontal).toString());
    }
    m_content += u"</tr></thead>\n"_s;

    m_content += u"<tbody>\n"_s;
    for (auto i = 0; i < model.rowCount(); i++) {
        m_content += u"<tr>"_s;
        for (auto j = 0; j < model.columnCount(); j++) {
            const auto index = model.index(i, j);
            const auto data = model.data(index, Qt::DisplayRole).toString().toHtmlEscaped();
            const auto alignment = model.data(index, Qt::TextAlignmentRole);

            QString textAlign = u"left"_s;
            if (alignment.isValid()) {
                switch (alignment.toInt()) {
                case Qt::AlignCenter:
                    textAlign = u"center"_s;
                    break;
                case Qt::AlignTrailing:
                    textAlign = u"right"_s;
                    break;
                }
            }

            m_content += u"<td align=\"%2\" style=\"padding: 2px;\">%1</td>"_s.arg(data, textAlign);
        }
        m_content += u"</tr>\n"_s;
    }
    m_content += u"</tbody></table>\n"_s;
}

QString Document::toHTML() const
{
    const auto result = DOCUMENT_TEMPLATE.arg(m_content);
    return result;
}

void Document::saveAs(const QString &fileName)
{
    m_doc->setHtml(toHTML());

    QPrinter printer;
    printer.setOutputFileName(fileName);
    printer.setFullPage(true);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF{10, 12, 10, 12});

    const qreal dpiX = qt_defaultDpiX();
    const qreal dpiY = qt_defaultDpiY();

    const int horizontalMargin = int((1 / 2.54) * dpiX);
    const int verticalMargin = int((1.2 / 2.54) * dpiY);
    QTextFrameFormat fmt = m_doc->rootFrame()->frameFormat();
    fmt.setLeftMargin(horizontalMargin);
    fmt.setRightMargin(horizontalMargin);
    fmt.setTopMargin(verticalMargin);
    fmt.setBottomMargin(verticalMargin);
    m_doc->rootFrame()->setFrameFormat(fmt);

    auto rect = QRectF(0, 0, printer.width(), printer.height());
    m_doc->setPageSize(rect.size());

    m_doc->print(&printer);
}

void Document::print()
{
    m_doc->setHtml(toHTML());

    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF{10, 12, 10, 12});

    QPrintDialog dialog(&printer);

    if (dialog.exec() == QDialog::Accepted) {
        const qreal dpiX = qt_defaultDpiX();
        const qreal dpiY = qt_defaultDpiY();

        const auto margins = printer.pageLayout().margins(QPageLayout::Inch);

        QTextFrameFormat fmt = m_doc->rootFrame()->frameFormat();
        fmt.setLeftMargin(margins.left() * dpiX);
        fmt.setRightMargin(margins.right() * dpiX);
        fmt.setTopMargin(margins.top() * dpiY);
        fmt.setBottomMargin(margins.bottom() * dpiY);
        m_doc->rootFrame()->setFrameFormat(fmt);

        const auto rect = QRectF(0, 0, printer.width(), printer.height());
        m_doc->setPageSize(QSizeF{rect.width() / (printer.logicalDpiX() / dpiX), rect.height() / (printer.logicalDpiY() / dpiY)});

        m_doc->print(&printer);
    }
}
