// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "document.h"

#include <QAbstractItemModel>
#include <QFile>

using namespace Qt::StringLiterals;

Document::Document()
{
    QFile file(u":/documents/default/document.html"_s);
    Q_ASSERT(file.open(QIODevice::ReadOnly));

    QTextStream stream(&file);

    m_html = stream.readAll();
}

void Document::addTitle(uint level, const QString &text)
{
    Q_ASSERT(level <= 6);

    m_content += u"<h%1>%2</h%1>\n"_s.arg(QString::number(level), text);
}

void Document::addTable(const QAbstractItemModel &model)
{
    m_content += u"<table>\n"_s;

    m_content += u"<thead><tr>\n"_s;
    for (auto i = 0; i < model.columnCount(); i++) {
        m_content += u"<th scope=\"col\">%1</th>\n"_s.arg(model.headerData(i, Qt::Horizontal).toString());
    }
    m_content += u"</tr></thead>\n"_s;

    m_content += u"<tbody>\n"_s;
    for (auto i = 0; i < model.rowCount(); i++) {
        m_content += u"<tr>"_s;
        for (auto j = 0; j < model.columnCount(); j++) {
            const auto index = model.index(i, j);
            const auto data = model.data(index, Qt::DisplayRole).toString();
            const auto alignment = model.data(index, Qt::TextAlignmentRole);

            QString textAlign = u"start"_s;
            if (alignment.isValid()) {
                switch (alignment.toInt()) {
                case Qt::AlignTrailing:
                    textAlign = u"end"_s;
                    break;
                }
            }

            m_content += u"<td style=\"text-align: %2;\">%1</td>"_s.arg(data, textAlign);
        }
        m_content += u"</tr>\n"_s;
    }
    m_content += u"</tbody></table>\n"_s;
}

QString Document::toHTML() const
{
    const auto result = m_html.arg(m_content);
    return result;
}
