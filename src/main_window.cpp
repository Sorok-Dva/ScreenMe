#include "include/main_window.h"
#include "include/utils.h"
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QJsonObject>
#include <QDebug>
#include "include/screenshotdisplay.h"
#include "include/utils.h"
#include "include/uglobalhotkeys.h"

MainWindow::MainWindow(ConfigManager* configManager, QWidget* parent)
    : QWidget(parent), configManager(configManager), isScreenshotDisplayed(false) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setGeometry(QGuiApplication::primaryScreen()->geometry());

    // Initialize UGlobalHotkeys
    hotkeyManager = new UGlobalHotkeys();

    QJsonObject config = configManager->loadConfig();
    QString screenshotHotkey = config["screenshot_hotkey"].toString();
    QString fullscreenHotkey = config["fullscreen_hotkey"].toString();

    if (!screenshotHotkey.isEmpty()) {
        hotkeyManager->registerHotkey(screenshotHotkey, 1);
    }

    if (!fullscreenHotkey.isEmpty()) {
        hotkeyManager->registerHotkey(fullscreenHotkey, 2);
    }

    connect(hotkeyManager, &UGlobalHotkeys::activated, this, &MainWindow::handleHotkeyActivated);
}

void MainWindow::takeScreenshot() {
    qDebug() << "takeScreenshot";
    if (isScreenshotDisplayed) return;

    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        qDebug() << "No primary screen found";
        return;
    }
    QPixmap originalPixmap = screen->grabWindow(0);
    ScreenshotDisplay* display = new ScreenshotDisplay(originalPixmap);
    connect(display, &ScreenshotDisplay::screenshotClosed, this, &MainWindow::handleScreenshotClosed);
    display->show();
    isScreenshotDisplayed = true;
}

void MainWindow::takeFullscreenScreenshot() {
    if (isScreenshotDisplayed) return;

    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        qDebug() << "No primary screen found";
        return;
    }
    QPixmap originalPixmap = screen->grabWindow(0);
    QJsonObject config = configManager->loadConfig();
    QString savePath = getUniqueFilePath(config["default_save_folder"].toString(), "fullscreen_screenshot", config["file_extension"].toString());
    originalPixmap.save(savePath);
}

void MainWindow::handleHotkeyActivated(size_t id) {
    if (id == 1) {
        takeScreenshot();
    }
    else if (id == 2) {
        takeFullscreenScreenshot();
    }
}

void MainWindow::handleScreenshotClosed() {
    isScreenshotDisplayed = false;
}