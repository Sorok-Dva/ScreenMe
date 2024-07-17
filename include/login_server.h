#ifndef LOGIN_SERVER_H
#define LOGIN_SERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

class LoginServer : public QObject
{
    Q_OBJECT
public:
    explicit LoginServer(QObject* parent = nullptr);
    ~LoginServer();

signals:
    void userLoggedIn();

private slots:
    void onNewConnection();
    void processMessage(const QString& message);

private:
    QWebSocketServer* m_webSocketServer;
    QList<QWebSocket*> m_clients;
};

#endif // LOGIN_SERVER_H
