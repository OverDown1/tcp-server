#include "server.h"

Server::Server(QObject *parent/*= nullptr*/) :
    QTcpServer (parent)
{
    quint16 port = 1236;

    if(!this->listen(QHostAddress::Any,port))
    {
        qDebug() << "Не удалось запустить сервер!";
    }
    else
    {
        qDebug() << "Прослушиваем порт: " << port << "...";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket();
    qintptr senderID = socketDescriptor + 1000;
    QString str = QString::number(senderID);
    QByteArray sendIP;
    sendIP.append(str);
    clients.insert(socket, senderID);
    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << "Неудачное подключение!: " << socketDescriptor << " client";
        return;
    }
    qDebug() << clients.value(socket) << " Соеденился!";

    socket->write(sendIP);

            connect(socket, SIGNAL(readyRead()), this, SLOT(readSock()));
           connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void Server::readSock()
{
    QTcpSocket *socket = (qobject_cast<QTcpSocket*>(sender()));
    socket->waitForReadyRead(500);

    QByteArray data;
    QByteArray sendingData;
    QString ID;
    /*
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_4);

    in >> data;*/
    data = socket->readAll();

    jsonDoc = QJsonDocument::fromJson(data, &jsonErr);
    if (jsonErr.errorString()=="no error occurred")
    {
        if(jsonDoc.object().value("type").toString() == "text"){
            ID.append(jsonDoc.object().value("recID").toString());
            sendingData.append(jsonDoc.object().value("message").toString());
        }
        else if(jsonDoc.object().value("type").toString() == "doc"){
            ID.append(jsonDoc.object().value("recID").toString());
            sendingData.append(jsonDoc.object().value("message").toString());
        }
    }
    else {
        qDebug() << "Ошибка c форматом передачи данных! " << jsonErr.errorString();
    }

    if (ID.isEmpty()){
        qDebug() << "Пустое ID";
        return;
    }
    else if(sendingData.isEmpty()){
        qDebug() << "Пустое сообщение";
        return;
    }
    qDebug() << clients.value(socket) << " отправил сообщение";

    for (QMap<QTcpSocket*, qintptr>::iterator it = clients.begin();it != clients.end();++it){
        if((ID.toInt() == it.value()) && socket != it.key()){
            QTcpSocket *sock = new QTcpSocket();
            sock = it.key();
            sock->write(sendingData);
        }
    }


}
void Server::disconnected()
{
    QTcpSocket *socket = (qobject_cast<QTcpSocket*>(sender()));
    if (socket == nullptr){
        return;
    }
    qDebug() << clients.value(socket) <<  " отсоеденился";
    socket->deleteLater();
    clients.remove(socket);
}

Server::~Server()
{

}
