#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QDebug>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDataStream>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
protected:
    void incomingConnection(qintptr socketDescriptor);
private:
    QMap<QTcpSocket*, qintptr> clients;
    QJsonDocument jsonDoc;
    QJsonParseError jsonErr;
public slots:
    void readSock();
    void disconnected();
signals:
    void error(QTcpSocket::SocketError socketerror);
};

#endif // SERVER_H
