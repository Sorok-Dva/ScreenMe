#include "include/utils.h"
#include "include/screenshotdisplay.h"
#include <QDir>
#include <QScreen>
#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStandardPaths>
#include <QSettings>
#include <QProcess>

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

QString getConfigFilePath(const QString& file) {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath(file);
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

void saveLoginInfo(const QString& id, const QString& email, const QString& nickname, const QString& token) {
    QString filePath = getConfigFilePath("login_info.json");
    QFile loginFile(filePath);
    if (loginFile.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObj;
        jsonObj["id"] = id;
        jsonObj["email"] = email;
        jsonObj["nickname"] = nickname;
        jsonObj["token"] = token;

        QJsonDocument jsonDoc(jsonObj);
        loginFile.write(jsonDoc.toJson());
    }
}

QString loadLoginInfo() {
    QString filePath = getConfigFilePath("login_info.json");
    QFile loginFile(filePath);
    if (loginFile.open(QIODevice::ReadOnly)) {
        QByteArray data = loginFile.readAll();
        return QString(data);
    }
    return QString();
}

void clearLoginInfo() {
    QString filePath = getConfigFilePath("login_info.json");
    QFile loginFile(filePath);
    loginFile.remove();
}


void setAutoStart(bool enable) {
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if (enable) {
        QString applicationName = QApplication::applicationName();
        QString applicationPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        settings.setValue(applicationName, applicationPath);
    } else {
        settings.remove(QApplication::applicationName());
    }
#elif defined(Q_OS_MAC)
    QString script;
    QString appName = QApplication::applicationName();
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    if (enable) {
        script = QString(
                     "tell application \"System Events\"\n"
                     "if not (exists login item \"%1\") then\n"
                     "make login item at end with properties {name:\"%1\", path:\"%2\", hidden:false}\n"
                     "end if\n"
                     "end tell"
                     ).arg(appName, appPath);
    } else {
        script = QString(
                     "tell application \"System Events\"\n"
                     "if (exists login item \"%1\") then\n"
                     "delete login item \"%1\"\n"
                     "end if\n"
                     "end tell"
                     ).arg(appName);
    }

    QProcess process;
    process.start("osascript", QStringList() << "-e" << script);
    process.waitForFinished();
#else
    // Unsupported platform
    Q_UNUSED(enable);
#endif
}
