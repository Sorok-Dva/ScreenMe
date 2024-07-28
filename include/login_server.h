#pragma once

#ifndef LOGIN_SERVER_H
#define LOGIN_SERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

class LoginServer : public QObject {
    Q_OBJECT
public:
    explicit LoginServer(QObject* parent = nullptr);

signals:
    void userLoggedIn(const QString& id, const QString& email, const QString& nickname, const QString& token);

public slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();

private:
    QWebSocketServer* webSocketServer;
    QList<QWebSocket*> clients;
};

#endif // LOGIN_SERVER_H
