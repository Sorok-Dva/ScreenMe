#include "../include/main_window.h"
#include "../include/utils.h"
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QJsonObject>
#include <QDebug>
#include <QStandardPaths>
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
    if (isScreenshotDisplayed) return;

    DesktopCapture capture = captureEntireDesktop();
    if (!capture.isValid()) {
        qWarning() << tr("Unable to capture desktop");
        return;
    }

    screenshotDisplay = new ScreenshotDisplay(capture.pixmap, capture.geometry, nullptr, configManager);
    connect(screenshotDisplay, &ScreenshotDisplay::screenshotClosed, this, &MainWindow::handleScreenshotClosed);
    screenshotDisplay->show();
    isScreenshotDisplayed = true;
}

void MainWindow::takeFullscreenScreenshot() {
    if (isScreenshotDisplayed) return;

    DesktopCapture capture = captureEntireDesktop();
    if (!capture.isValid()) {
        qWarning() << tr("Unable to capture desktop");
        return;
    }
    QJsonObject config = configManager->loadConfig();
    QString folder = config["default_save_folder"].toString();
    if (folder.isEmpty()) {
        folder = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
    QString extension = config["file_extension"].toString();
    if (extension.isEmpty()) {
        extension = QStringLiteral("png");
    }

    QString savePath = getUniqueFilePath(folder, "fullscreen_screenshot", extension);
    if (!capture.pixmap.save(savePath)) {
        qWarning() << "Failed to save fullscreen screenshot to" << savePath;
        return;
    }

    emit fullscreenSaved(savePath);
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
    if (screenshotDisplay) {
        screenshotDisplay->deleteLater();
        screenshotDisplay = nullptr;
    }
}
