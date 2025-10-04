#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include <iostream>
#include <QDebug>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QDesktopServices>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSharedMemory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QDir>
#include <memory>
#include "include/options_window.h"
#include "include/config_manager.h"
#include "include/login_loader.h"
#include "include/login_server.h"
#include "include/main_window.h"
#include "include/utils.h"
#include "include/credits_dialog.h"
#include "include/simpletranslator.h"
#ifdef Q_OS_WIN
#include "include/hotkeyEventFilter.h"
#endif


#define SHARED_MEM_KEY "ScreenMeSharedMemory"

static void showAboutDialog() {
    CreditsDialog dialog;
    dialog.exec();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    // If built as a console app on Windows we can detach from the console.

    app.setWindowIcon(QIcon(":/resources/icon.png"));

    QSharedMemory sharedMemory(SHARED_MEM_KEY);
    if (!sharedMemory.create(1)) {
        QMessageBox::warning(nullptr,
                              QObject::tr("ScreenMe is already running"),
                              QObject::tr("An instance of this application is already running. Please quit the existing ScreenMe process first."));
        return 1;
    }

    QString jsonStr = loadLoginInfo();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject loginInfo = jsonDoc.object();

    ConfigManager configManager("config.json");
    QJsonObject config = configManager.loadConfig();

    const QString language = config["language"].toString(QStringLiteral("en"));
    std::unique_ptr<SimpleTranslator> translator;
    if (!language.isEmpty() && language.toLower() != QStringLiteral("en")) {
        auto simple = std::make_unique<SimpleTranslator>();
        if (simple->loadLanguage(language)) {
            translator = std::move(simple);
            app.installTranslator(translator.get());
        } else {
            qWarning() << "Unable to load translation for" << language;
        }
    }

    QSystemTrayIcon trayIcon(QIcon(":/resources/icon.png"));
    QMenu trayMenu;

#ifdef Q_OS_WIN
    const QVariant startWithSystemVar = config["start_with_system"].toBool();
    if (startWithSystemVar.isValid() && startWithSystemVar.toBool()) {
        setAutoStart(true);
    }
    else {
        setAutoStart(false);
    }
#endif

    MainWindow mainWindow(&configManager);
    mainWindow.checkForUpdates(false);
    mainWindow.hide();

    QAction loginAction(QObject::tr("Login to ScreenMe"), &trayMenu);
    QAction takeScreenshotAction(QObject::tr("Take Screenshot"), &trayMenu);
    QAction takeFullscreenScreenshotAction(QObject::tr("Take Fullscreen Screenshot"), &trayMenu);
    QAction aboutAction(QObject::tr("Credits"), &trayMenu);
    QAction helpAction(QObject::tr("❓Help"), &trayMenu);
    QAction reportBugAction(QObject::tr("🛠️ Report a bug"), &trayMenu);
    QAction optionsAction(QObject::tr("Options"), &trayMenu);
    QAction checkUpdateAction(QObject::tr("Check for update"), &trayMenu);
    QAction exitAction(QObject::tr("Exit"), &trayMenu);

    QAction myGalleryAction(QObject::tr("My Gallery"), &trayMenu);
    QAction logoutAction(QObject::tr("Logout"), &trayMenu);

    if (loginInfo.isEmpty()) {
        trayMenu.addAction(&loginAction);
        trayMenu.addSeparator();
    }
    else {
        QString nickname = loginInfo["nickname"].toString();
        trayMenu.addAction(&myGalleryAction);
        trayMenu.addAction(&logoutAction);
        trayMenu.addSeparator();
        myGalleryAction.setText(QObject::tr("My Gallery (%1)").arg(nickname));
    }

    trayMenu.addAction(&takeScreenshotAction);
    trayMenu.addAction(&takeFullscreenScreenshotAction);
    trayMenu.addSeparator();
    trayMenu.addAction(&aboutAction);
    trayMenu.addAction(&helpAction);
    trayMenu.addAction(&reportBugAction);
    trayMenu.addSeparator();
    trayMenu.addAction(&optionsAction);
    trayMenu.addAction(&checkUpdateAction);
    trayMenu.addAction(&exitAction);
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setToolTip(QObject::tr("Press the configured key combination to take a screenshot"));

    LoginServer loginServer;
    LoginLoader loginLoader;

    QObject::connect(&loginAction, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl(SCREEN_ME_HOST + "/login"));
        loginLoader.show();
    });

    QObject::connect(&loginServer, &LoginServer::userLoggedIn, &loginLoader, &LoginLoader::close);
    QObject::connect(&loginServer, &LoginServer::userLoggedIn, [&](const QString& id, const QString& email, const QString& nickname, const QString& token) {
        saveLoginInfo(id, email, nickname, token);

        trayIcon.showMessage(QObject::tr("Login Successful"),
                             QObject::tr("Connected as %1").arg(nickname),
                             QSystemTrayIcon::Information,
                             3000);

        trayMenu.removeAction(&loginAction);
        trayMenu.insertAction(&takeScreenshotAction, &myGalleryAction);
        trayMenu.insertAction(&takeScreenshotAction, &logoutAction);
        trayMenu.insertSeparator(&logoutAction);
        myGalleryAction.setText(QObject::tr("My Gallery (%1)").arg(nickname));
    });

    QObject::connect(&myGalleryAction, &QAction::triggered, [&]() {
        QString jsonStr = loadLoginInfo();
        if (!jsonStr.isEmpty()) {
            QDesktopServices::openUrl(QUrl(SCREEN_ME_HOST + "/gallery"));
        }
    });

    QObject::connect(&logoutAction, &QAction::triggered, [&]() {
        clearLoginInfo();
        trayMenu.removeAction(&myGalleryAction);
        trayMenu.removeAction(&logoutAction);
        trayMenu.insertAction(&takeScreenshotAction, &loginAction);
        trayMenu.insertSeparator(&loginAction);
    });

    QObject::connect(&exitAction, &QAction::triggered, &app, &QApplication::quit);

    QObject::connect(&takeScreenshotAction, &QAction::triggered, [&]() {
        mainWindow.takeScreenshot();
    });

    QObject::connect(&takeFullscreenScreenshotAction, &QAction::triggered, [&]() {
        mainWindow.takeFullscreenScreenshot();
    });

    QObject::connect(&mainWindow, &MainWindow::fullscreenSaved, [&](const QString& path) {
        trayIcon.showMessage(QObject::tr("Screenshot saved"),
                             QObject::tr("Fullscreen capture stored at %1").arg(QDir::toNativeSeparators(path)),
                             QSystemTrayIcon::Information,
                             3000);
    });

    QObject::connect(&aboutAction, &QAction::triggered, [&]() {
        showAboutDialog();
    });

    QObject::connect(&helpAction, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl(SCREEN_ME_HOST + "/help"));
    });

    QObject::connect(&reportBugAction, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl("https://github.com/Sorok-Dva/ScreenMe/issues"));
    });

    QObject::connect(&optionsAction, &QAction::triggered, [&]() {
        OptionsWindow optionsWindow(&configManager);
        QObject::connect(&optionsWindow, &OptionsWindow::reloadHotkeys, &mainWindow, &MainWindow::reloadHotkeys);
        optionsWindow.exec();
    });

    QObject::connect(&checkUpdateAction, &QAction::triggered, [&]() {
        QJsonObject mutableConfig = configManager.loadConfig();
        mutableConfig["skipVersion"] = "";
        configManager.saveConfig(mutableConfig);
        mainWindow.checkForUpdates(true);
    });

    trayIcon.show();

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            mainWindow.takeScreenshot();
        }
    });

    QObject::connect(&mainWindow, &MainWindow::screenshotClosed, [&]() {
        mainWindow.handleScreenshotClosed();
    });

#ifdef Q_OS_WIN
    HotkeyEventFilter hotkeyEventFilter(&mainWindow);
    app.installNativeEventFilter(&hotkeyEventFilter);

    QObject::connect(&hotkeyEventFilter, &HotkeyEventFilter::hotkeyPressed, [&](quint32 id) {
        mainWindow.handleHotkeyActivated(id);
        std::cout << "Global hotkey pressed!" << std::endl;
    });
#endif

    return app.exec();
}
