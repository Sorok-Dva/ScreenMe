#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFile>
#include <QProcess>
#include <QProgressDialog>
#include "include/main_window.h"
#include "include/utils.h"

void MainWindow::checkForUpdates() {
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onUpdateCheckFinished);

    QUrl url("https://screen-me.cloud/update.json");
    QNetworkRequest request(url);
    manager->get(request);
}

void MainWindow::onUpdateCheckFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            QString latestVersion = jsonObj.value("version").toString();
            QString currentVersion = VERSION;

            ConfigManager configManager("config.json");
            QJsonObject config = configManager.loadConfig();

            if (latestVersion > currentVersion && config["skipVersion"] != latestVersion) {
                QString downloadUrl = jsonObj.value("download_url").toString();

                QMessageBox::StandardButton replyButton;
                replyButton = QMessageBox::question(this,
                    "New update available !",
                    QString("A new update (%1) has been released. Do you want to download and install now ?").arg(latestVersion),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Ignore);
                if (replyButton == QMessageBox::Yes) {
                    downloadUpdate(downloadUrl);
                } else if (replyButton == QMessageBox::Ignore) {
                    config["skipVersion"] = latestVersion;

                    configManager.saveConfig(config);
                }
            }
        }
    }
    reply->deleteLater();
}

void MainWindow::downloadUpdate(const QString& downloadUrl) {
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QUrl url(downloadUrl);
    if (!url.isValid()) {
        qDebug() << "Invalid URL: " << url;
        return;
    }
    QNetworkRequest request(url);

    QProgressDialog* progressDialog = new QProgressDialog("Downloading update...", "Cancel", 0, 100, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);

    QNetworkReply* networkReply = manager->get(request);

    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onDownloadFinished);
    connect(networkReply, &QNetworkReply::downloadProgress, this, [progressDialog](qint64 bytesReceived, qint64 bytesTotal) {
        if (bytesTotal > 0) {
            progressDialog->setMaximum(100);
            progressDialog->setValue(static_cast<int>((bytesReceived * 100) / bytesTotal));
        }
    });
    connect(progressDialog, &QProgressDialog::canceled, networkReply, &QNetworkReply::abort);
}

void MainWindow::onDownloadFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QFile file("update.exe");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();

            QMessageBox::StandardButton replyButton;
            replyButton = QMessageBox::question(this,
                "Download finished",
                "The update has been downloaded. Do you want to install now ?",
                QMessageBox::Yes | QMessageBox::No);
            if (replyButton == QMessageBox::Yes) {
                QProcess::startDetached("update.exe");
                QApplication::exit(0);
            }
        }
    }
    reply->deleteLater();
} 
