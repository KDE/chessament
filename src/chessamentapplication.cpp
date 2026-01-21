// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "chessamentapplication.h"
#include <KAuthorized>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

ChessamentApplication::ChessamentApplication(QObject *parent)
    : AbstractKirigamiApplication(parent)
{
    setupActions();
}

void ChessamentApplication::setupActions()
{
    AbstractKirigamiApplication::setupActions();

    auto actionName = "file_new"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &ChessamentApplication::newTournament);
        action->setText(i18nc("@action:inmenu", "New tournament…"));
        action->setIcon(QIcon::fromTheme(u"document-new-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
        mainCollection()->setDefaultShortcut(action, Qt::Key_New);
    }

    actionName = "file_open"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::open(this, &ChessamentApplication::openTournament, this);
        mainCollection()->addAction(action->objectName(), action);
    }

    actionName = "file_save_as"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::saveAs(this, &ChessamentApplication::saveTournamentAs, this);
        mainCollection()->addAction(action->objectName(), action);
    }

    actionName = "import_trf"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &ChessamentApplication::importTrf);
        action->setText(i18nc("@action:inmenu", "Import tournament report…"));
        action->setIcon(QIcon::fromTheme(u"document-import-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
        mainCollection()->setDefaultShortcut(action, Qt::CTRL | Qt::Key_I);
    }

    actionName = "export_trf"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &ChessamentApplication::exportTrf);
        action->setText(i18nc("@action:inmenu", "Export tournament report…"));
        action->setIcon(QIcon::fromTheme(u"document-export-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
    }

    readSettings();
}

#include "moc_chessamentapplication.cpp"
