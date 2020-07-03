#include "mqttbridgeserverthread.h"
#include <QtCore/QDataStream>
#include <QBuffer>
#include <QtEndian>

MQTTBridgeServerThread::MQTTBridgeServerThread(qintptr socketDescriptor, QObject *parent) :
    QThread(parent),
    socketDescriptor(socketDescriptor) {

}

void MQTTBridgeServerThread::run() {
    qDebug() << "Thread started ";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor)) {
        qDebug() << "error";
        emit error(socket->error());
        return;
    }
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    //static const char mydata[] = {'\x20'};
    //QByteArray x = QByteArray::fromRawData(mydata, sizeof(mydata));
    //QDataStream inToX(&x, QIODevice::WriteOnly);
    //QByteArray x;
    //x[0] = 0x20;
    //socket->write(x);
    //qDebug() << x.toInt() << " " << x.size();
    exec();
}

void MQTTBridgeServerThread::readyRead() {
    QByteArray message = socket->readAll();
    qDebug() << socketDescriptor << " Data in: " << message;
    int messageType = static_cast<quint8>(message[0]) >> 4;
    qDebug() << messageType << " = messageType";
    //works
    static const char mydata[] = {'\x20','\x02', '\x00', '\x00'};
    QByteArray response = QByteArray::fromRawData(mydata, sizeof(mydata));
    QDataStream in(&response, QIODevice::ReadWrite);
    int command = static_cast<quint8>(response[0]);
    qDebug() << command << " " << response << " " << sizeof(mydata) << " " << response.size();
    //below works too
    QByteArray response2 = QByteArray::fromHex(QString("20020000").toUtf8());
    socket->write(response2);
    //exit(0);
}

void MQTTBridgeServerThread::disconnected() {
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}
