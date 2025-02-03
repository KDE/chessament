// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>
#include <QtGlobal>
#include <QtQml/QQmlExtensionPlugin>

#include "version-chessament.h"
#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedQmlContext>
#include <KLocalizedString>
#include <QCoroQml>

#include "controller.h"

#ifdef Q_OS_WINDOWS
#include <QFont>
#include <Windows.h>
#endif

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    KIconTheme::initTheme();
    QIcon::setFallbackThemeName("breeze"_L1);
    QApplication app(argc, argv);

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(u"org.kde.desktop"_s);
    }

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    QFont font(u"Segoe UI Emoji"_s);
    font.setPointSize(10);
    font.setHintingPreference(QFont::PreferNoHinting);
    app.setFont(font);
#endif

    KLocalizedString::setApplicationDomain("chessament");

    KAboutData aboutData(
        // The program name used internally.
        u"chessament"_s,
        // A displayable program name string.
        i18nc("@title", "Chessament"),
        // The program version string.
        QStringLiteral(CHESSAMENT_VERSION_STRING),
        // Short description of what the app does.
        i18n("Chess Tournament Manager"),
        // The license this code is released under.
        KAboutLicense::GPL_V3,
        // Copyright Statement.
        i18n("© 2024 Manuel Alcaraz Zambrano"));
    aboutData.addAuthor(i18nc("@info:credit", "Manuel Alcaraz Zambrano"),
                        i18nc("@info:credit", "Author & Maintainer"),
                        u"manuelalcarazzam@gmail.com"_s,
                        u"https://alcarazzam.dev"_s);
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(u"org.kde.chessament"_s));

    QCommandLineParser parser;

    QCommandLineOption trfFile("import-trf"_L1, i18n("Import Tournament Report File."), u"file"_s);
    parser.addOption(trfFile);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QQmlApplicationEngine engine;

    QCoro::Qml::registerTypes();

    KLocalization::setupLocalizedContext(&engine);
    engine.loadFromModule("org.kde.chessament", u"Main"_s);

    if (parser.isSet(trfFile)) {
        const auto fileName = parser.value(trfFile);
        auto *controller = engine.singletonInstance<Controller *>("org.kde.chessament", "Controller");
        controller->importTrf(QUrl::fromLocalFile(fileName));
    }

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
