#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <include/options_window.h>
#include <include/config_manager.h>
#include <include/main_window.h>
#include <Windows.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ConfigManager configManager("resources/config.json");
    QSystemTrayIcon trayIcon(QIcon("resources/icon.png"));
    QMenu trayMenu;

    QAction takeScreenshotAction("Take Screenshot", &trayMenu);
    QAction takeFullscreenScreenshotAction("Take Fullscreen Screenshot", &trayMenu);
    QAction optionsAction("Options", &trayMenu);
    QAction exitAction("Exit", &trayMenu);

    trayMenu.addAction(&takeScreenshotAction);
    trayMenu.addAction(&takeFullscreenScreenshotAction);
    trayMenu.addAction(&optionsAction);
    trayMenu.addAction(&exitAction);
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setToolTip("Press the configured key combination to take a screenshot");

    MainWindow mainWindow(&configManager);
    mainWindow.hide(); // Ensure the main window is hidden

    QObject::connect(&takeScreenshotAction, &QAction::triggered, [&]() {
        mainWindow.takeScreenshot();
    });

    QObject::connect(&takeFullscreenScreenshotAction, &QAction::triggered, [&]() {
        mainWindow.takeFullscreenScreenshot();
    });

    QObject::connect(&optionsAction, &QAction::triggered, [&]() {
        OptionsWindow optionsWindow(&configManager);
        optionsWindow.exec();
    });

    QObject::connect(&exitAction, &QAction::triggered, &app, &QApplication::quit);

    trayIcon.show();

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            mainWindow.takeScreenshot(); // Take screenshot on left-click
        }
    });

    QObject::connect(&mainWindow, &MainWindow::screenshotClosed, [&]() {
        mainWindow.handleScreenshotClosed();
    });

    /*MSG msg;
    while (true)
    {
        // Wait for messages
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            // Check if the message is for a hotkey
            if (msg.message == WM_HOTKEY)
            {
                std::cout << "Global hotkey pressed!" << std::endl;
            }

            // Translate and dispatch the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }*/

    return app.exec();
}
