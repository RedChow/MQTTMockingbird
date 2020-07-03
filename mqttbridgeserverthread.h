#ifndef MQTTBRIDGESERVERTHREAD_H
#define MQTTBRIDGESERVERTHREAD_H

#include <QObject>
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include <QDebug>

class MQTTBridgeServerThread: public QThread
{
    Q_OBJECT
    public:
        MQTTBridgeServerThread(qintptr socketDescriptor, QObject *parent);
        void run() override;

    signals:
        void error(QTcpSocket::SocketError socketError);

    public slots:
        void readyRead();
        void disconnected();

    private:
        qintptr socketDescriptor;
        QTcpSocket *socket;
};

#endif // MQTTBRIDGESERVERTHREAD_H
