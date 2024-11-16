#include "../include/main_window.h"
#include "../include/utils.h"
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QJsonObject>
#include <QDebug>
#include "../include/options_window.h"
#include "../include/screenshotdisplay.h"
#include "../include/uglobalhotkeys.h"

MainWindow::MainWindow(ConfigManager* configManager, QWidget* parent)
    : QMainWindow(parent), configManager(configManager), isScreenshotDisplayed(false) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setGeometry(QGuiApplication::primaryScreen()->geometry());

    // Initialize UGlobalHotkeys
    hotkeyManager = new UGlobalHotkeys(this);

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

void MainWindow::reloadHotkeys() {
    hotkeyManager->unregisterAllHotkeys();

    QJsonObject config = configManager->loadConfig();
    QString screenshotHotkey = config["screenshot_hotkey"].toString();
    QString fullscreenHotkey = config["fullscreen_hotkey"].toString();

    if (!screenshotHotkey.isEmpty()) {
        hotkeyManager->registerHotkey(screenshotHotkey, 1);
    }

    if (!fullscreenHotkey.isEmpty()) {
        hotkeyManager->registerHotkey(fullscreenHotkey, 2);
    }
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
    screenshotDisplay = new ScreenshotDisplay(originalPixmap, nullptr, configManager);
    connect(screenshotDisplay, &ScreenshotDisplay::screenshotClosed, this, &MainWindow::handleScreenshotClosed);
    screenshotDisplay->show();
    isScreenshotDisplayed = true;
}

void MainWindow::takeFullscreenScreenshot() {
    qDebug() << "takeFullscreenScreenshot";
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
    qDebug() << "handleScreenshotClosed";
    isScreenshotDisplayed = false;
    if (screenshotDisplay) {
        screenshotDisplay->deleteLater();
        screenshotDisplay = nullptr;
    }
}
