#include "include/login_server.h"
#include <QDebug>

LoginServer::LoginServer(QObject* parent)
    : QObject(parent),
    m_webSocketServer(new QWebSocketServer(QStringLiteral("Login Server"), QWebSocketServer::NonSecureMode, this))
{
    if (m_webSocketServer->listen(QHostAddress::Any, 6789)) {
        qDebug() << "Login Server listening on port 6789";
        connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &LoginServer::onNewConnection);
    }
}

LoginServer::~LoginServer()
{
    m_webSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void LoginServer::onNewConnection()
{
    QWebSocket* client = m_webSocketServer->nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &LoginServer::processMessage);
    connect(client, &QWebSocket::disconnected, client, &QWebSocket::deleteLater);

    m_clients << client;
}

void LoginServer::processMessage(const QString& message)
{
    if (message == "User connected") {
        emit userLoggedIn();
    }
}
