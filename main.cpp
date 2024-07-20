#include <Windows.h>
#include <iostream>
#include <QMainWindow>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QDesktopServices>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <include/options_window.h>
#include <include/config_manager.h>
#include "include/login_loader.h"
#include "include/login_server.h"
#include <include/main_window.h>
#include <include/utils.h>
#include <include/hotkeyEventFilter.h>

using namespace std;

const QString VERSION = "1.0.2";

static void showAboutDialog() {
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About ScreenMe");
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText(
        "<h1>ScreenMe<h1>"
        "Version <b>" + VERSION + "</b><br><br>"
        "<span style=\"color: green\">Contribute on GitHub ! </span> : <a href=\"https://github.com/Sorok-Dva/ScreenMe\">Github Repository</a><br><br>"
    );
    aboutBox.setInformativeText(
        "Terms of use of ScreenMe : <a href=\"" + SCREEN_ME_HOST + "/terms-of-use\">" + SCREEN_ME_HOST + "/terms-of-use</a><br><br>"
        "© 2024 Developed by <a href=\"https://github.com/Sorok-Dva\">Сорок два</a>. <b>All rights reserved.</b>"
    );
    aboutBox.setIconPixmap(QPixmap("resources/icon.png"));
    aboutBox.exec();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
    int argc = 0;
    QApplication app(argc, 0);
#ifdef _WIN32
    // Ensure the console window does not appear on Windows
    FreeConsole();
#endif

    QString jsonStr = loadLoginInfo();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject loginInfo = jsonDoc.object();

    ConfigManager configManager("resources/config.json");
    QSystemTrayIcon trayIcon(QIcon("resources/icon.png"));
    QMenu trayMenu;

    QAction loginAction("Login to ScreenMe", &trayMenu);
    QAction takeScreenshotAction("Take Screenshot", &trayMenu);
    QAction takeFullscreenScreenshotAction("Take Fullscreen Screenshot", &trayMenu);
    QAction aboutAction("About...", &trayMenu);
    QAction helpAction("❓Help", &trayMenu);
    QAction reportBugAction("🛠️ Report a bug", &trayMenu);
    QAction optionsAction("Options", &trayMenu);
    QAction exitAction("Exit", &trayMenu);

    QAction myGalleryAction("My Gallery", &trayMenu);
    QAction logoutAction("Logout", &trayMenu);

    if (loginInfo.isEmpty()) {
        trayMenu.addAction(&loginAction);
        trayMenu.addSeparator();
    }
    else {
        QString nickname = loginInfo["nickname"].toString();
        trayMenu.addAction(&myGalleryAction);
        trayMenu.addAction(&logoutAction);
        trayMenu.addSeparator();
        myGalleryAction.setText("My Gallery (" + nickname + ")");
    }

    trayMenu.addAction(&takeScreenshotAction);
    trayMenu.addAction(&takeFullscreenScreenshotAction);
    trayMenu.addSeparator();
    trayMenu.addAction(&aboutAction);
    trayMenu.addAction(&helpAction);
    trayMenu.addAction(&reportBugAction);
    trayMenu.addSeparator();
    trayMenu.addAction(&optionsAction);
    trayMenu.addAction(&exitAction);
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setToolTip("Press the configured key combination to take a screenshot");

    LoginServer loginServer;
    LoginLoader loginLoader;

    QObject::connect(&loginAction, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl(SCREEN_ME_HOST + "/login"));
        loginLoader.show();
    });

    QObject::connect(&loginServer, &LoginServer::userLoggedIn, &loginLoader, &LoginLoader::close);
    QObject::connect(&loginServer, &LoginServer::userLoggedIn, [&](const QString& id, const QString& email, const QString& nickname, const QString& token) {
        saveLoginInfo(id, email, nickname, token);

        trayIcon.showMessage("Login Successful", "Connected as " + nickname, QSystemTrayIcon::Information, 3000);

        trayMenu.removeAction(&loginAction);
        trayMenu.insertAction(&takeScreenshotAction, &myGalleryAction);
        trayMenu.insertAction(&takeScreenshotAction, &logoutAction);
        trayMenu.insertSeparator(&logoutAction);
        myGalleryAction.setText("My Gallery (" + nickname + ")");
    });

    QObject::connect(&myGalleryAction, &QAction::triggered, [&]() {
        QString jsonStr = loadLoginInfo();
        if (!jsonStr.isEmpty()) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
            QJsonObject loginInfo = jsonDoc.object();
            QString nickname = loginInfo["nickname"].toString();
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

    QObject::connect(&exitAction, &QAction::triggered, [&]() {
        qApp->exit();
    });

    MainWindow mainWindow(&configManager);
    mainWindow.hide();

    QObject::connect(&takeScreenshotAction, &QAction::triggered, [&]() {
        mainWindow.takeScreenshot();
    });

    QObject::connect(&takeFullscreenScreenshotAction, &QAction::triggered, [&]() {
        mainWindow.takeFullscreenScreenshot();
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
        optionsWindow.exec();
    });

    trayIcon.show();

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            mainWindow.takeScreenshot(); // Take screenshot on left-click
        }
    });

    QObject::connect(&mainWindow, &MainWindow::screenshotClosed, [&]() {
        mainWindow.handleScreenshotClosed();
    });

    HotkeyEventFilter hotkeyEventFilter(&mainWindow);
    app.installNativeEventFilter(&hotkeyEventFilter);

    QObject::connect(&hotkeyEventFilter, &HotkeyEventFilter::hotkeyPressed, [&](quint32 id) {
        mainWindow.handleHotkeyActivated(id);
        std::cout << "Global hotkey pressed!" << std::endl;
    });

    return app.exec();
}
