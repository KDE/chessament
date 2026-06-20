// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "chessamentapplication.h"
#include <KAuthorized>
#include <KLocalizedString>
#include <KStandardActions>

using namespace Qt::StringLiterals;

ChessamentApplication::ChessamentApplication(QObject *parent)
    : AbstractKirigamiApplication(parent)
{
    setupActions();
}

void ChessamentApplication::setupActions()
{
    AbstractKirigamiApplication::setupActions();

    auto actionName = u"file_new"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &ChessamentApplication::newTournament);
        action->setText(i18nc("@action:inmenu", "New tournament…"));
        action->setIcon(QIcon::fromTheme(u"document-new-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
        KirigamiActionCollection::setDefaultShortcut(action, Qt::CTRL | Qt::Key_N);
    }

    actionName = u"file_open"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::open(this, &ChessamentApplication::openTournament, this);
        mainCollection()->addAction(action->objectName(), action);
    }

    actionName = u"file_save_as"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::saveAs(this, &ChessamentApplication::saveTournamentAs, this);
        mainCollection()->addAction(action->objectName(), action);
    }

    actionName = u"import_trf"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &ChessamentApplication::importTrf);
        action->setText(i18nc("@action:inmenu", "Import tournament report…"));
        action->setIcon(QIcon::fromTheme(u"document-import-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
        KirigamiActionCollection::setDefaultShortcut(action, Qt::CTRL | Qt::Key_I);
    }

    actionName = u"export_trf"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &ChessamentApplication::exportTrf);
        action->setText(i18nc("@action:inmenu", "Export tournament report…"));
        action->setIcon(QIcon::fromTheme(u"document-export-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
    }

    actionName = KStandardActions::name(KStandardActions::HelpContents);
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::helpContents(this, &ChessamentApplication::openHandbook, this);
        mainCollection()->addAction(actionName, action);
    }

    actionName = KStandardActions::name(KStandardActions::ReportBug);
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::reportBug(this, &ChessamentApplication::reportBug, this);
        mainCollection()->addAction(actionName, action);
    }

    readSettings();
}

#include "moc_chessamentapplication.cpp"
