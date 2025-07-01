// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

class QAbstractItemModel;
class QTextDocument;

class Document
{
public:
    explicit Document();

    void addTitle(uint level, const QString &text);
    void addTable(const QAbstractItemModel &model);

    QString toHTML() const;
    void saveAs(const QString &fileName);
    void print();

private:
    std::unique_ptr<QTextDocument> m_doc;

    QString m_content;
};
