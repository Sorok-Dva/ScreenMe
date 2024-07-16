#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QScreen>
#include <QPixmap>
#include <QWidget>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QShortcut>
#include <QGraphicsOpacityEffect>
#include "include/options_window.h"
#include "include/config_manager.h"

class ScreenshotDisplay : public QWidget {
    Q_OBJECT
public:
    explicit ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setGeometry(QApplication::primaryScreen()->geometry());
        QLabel* label = new QLabel(this);
        label->setPixmap(pixmap);
        label->setScaledContents(true);
        label->setGeometry(this->geometry());

        // Assombrir l'image
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(label);
        effect->setOpacity(0.5); // 50% d'opacité
        label->setGraphicsEffect(effect);

        QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
        connect(escapeShortcut, &QShortcut::activated, this, &ScreenshotDisplay::close);

        showFullScreen();
    }
};

QString getUniqueFilePath(const QString& folder, const QString& baseName, const QString& extension) {
    QDir dir(folder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    int i = 1;
    QString filePath;
    do {
        filePath = QString("%1/%2-%3.%4").arg(folder).arg(baseName).arg(i).arg(extension);
        i++;
    } while (QFile::exists(filePath));

    return filePath;
}

void CaptureScreenshot(const QString& savePath) {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        qDebug() << "No primary screen found";
        return;
    }
    QPixmap originalPixmap = screen->grabWindow(0);
    originalPixmap.save(savePath);
}

void displayScreenshotOnScreen(const QPixmap& pixmap) {
    ScreenshotDisplay* displayWidget = new ScreenshotDisplay(pixmap);
    displayWidget->setGeometry(QApplication::primaryScreen()->geometry());
    displayWidget->show();
}

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
    trayIcon.setToolTip("Appuyez sur la combinaison de touches configurée pour prendre une capture d'écran");

    QObject::connect(&takeScreenshotAction, &QAction::triggered, [&]() {
        QScreen* screen = QGuiApplication::primaryScreen();
        if (!screen) {
            qDebug() << "No primary screen found";
            return;
        }
        QPixmap originalPixmap = screen->grabWindow(0);
        displayScreenshotOnScreen(originalPixmap);
    });

    QObject::connect(&takeFullscreenScreenshotAction, &QAction::triggered, [&]() {
        QScreen* screen = QGuiApplication::primaryScreen();
        if (!screen) {
            qDebug() << "No primary screen found";
            return;
        }
        QPixmap originalPixmap = screen->grabWindow(0);
        QJsonObject config = configManager.loadConfig();
        QString savePath = getUniqueFilePath(config["default_save_folder"].toString(), "fullscreen_screenshot", config["file_extension"].toString());
        originalPixmap.save(savePath);
    });

    QObject::connect(&optionsAction, &QAction::triggered, [&]() {
        OptionsWindow optionsWindow(&configManager);
        optionsWindow.exec();
    });

    QObject::connect(&exitAction, &QAction::triggered, &app, &QApplication::quit);

    trayIcon.show();

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            // Handle left-click on tray icon if needed
        }
    });

    // Load hotkeys from config
    QJsonObject config = configManager.loadConfig();
    QString screenshotHotkey = config["screenshot_hotkey"].toString();
    QString fullscreenHotkey = config["fullscreen_hotkey"].toString();

    QShortcut* screenshotShortcut = new QShortcut(QKeySequence(screenshotHotkey), &app);
    QObject::connect(screenshotShortcut, &QShortcut::activated, [&]() {
        QScreen* screen = QGuiApplication::primaryScreen();
        if (!screen) {
            qDebug() << "No primary screen found";
            return;
        }
        QPixmap originalPixmap = screen->grabWindow(0);
        displayScreenshotOnScreen(originalPixmap);
    });

    QShortcut* fullscreenScreenshotShortcut = new QShortcut(QKeySequence(fullscreenHotkey), &app);
    QObject::connect(fullscreenScreenshotShortcut, &QShortcut::activated, [&]() {
        QScreen* screen = QGuiApplication::primaryScreen();
        if (!screen) {
            qDebug() << "No primary screen found";
            return;
        }
        QPixmap originalPixmap = screen->grabWindow(0);
        // Save the screenshot directly for fullscreen hotkey
        QString savePath = getUniqueFilePath(config["default_save_folder"].toString(), "fullscreen_screenshot", config["file_extension"].toString());
        originalPixmap.save(savePath);
    });

    return app.exec();
}

#include "main.moc"
