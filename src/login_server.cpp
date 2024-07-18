#include "include/login_server.h"
#include <QDebug>

LoginServer::LoginServer(QObject* parent)
    : QObject(parent),
    webSocketServer(new QWebSocketServer(QStringLiteral("Login Server"),
        QWebSocketServer::NonSecureMode, this)) {
    if (webSocketServer->listen(QHostAddress::LocalHost, 4242)) {
        connect(webSocketServer, &QWebSocketServer::newConnection,
            this, &LoginServer::onNewConnection);
    }
    else {
        qDebug() << "Failed to start WebSocket server !";
    }
}

void LoginServer::onNewConnection() {
    QWebSocket* client = webSocketServer->nextPendingConnection();
    clients << client;

    connect(client, &QWebSocket::textMessageReceived,
        this, &LoginServer::processTextMessage);
    connect(client, &QWebSocket::disconnected,
        this, &LoginServer::socketDisconnected);
}

void LoginServer::processTextMessage(QString message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "Received invalid JSON";
        return;
    }

    QJsonObject obj = doc.object();
    if (obj["type"].toString() == "login") {
        QJsonObject data = obj["data"].toObject();
        QString id = data["id"].toString();
        QString nickname = data["nickname"].toString();
        QString email = data["email"].toString();
        emit userLoggedIn(id, nickname, email);
    }
    else {
        qDebug() << "Received unknown message type";
    }
}

void LoginServer::socketDisconnected() {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}
