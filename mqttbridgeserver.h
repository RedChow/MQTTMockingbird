#ifndef MQTTBRIDGESERVER_H
#define MQTTBRIDGESERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QSignalMapper>
#include "mqttbridgeserverthread.h"
#include "clientsock.h"


class MQTTBridgeServer : public QTcpServer
{
Q_OBJECT
signals:
    void serverConnected();
    void itemAdded();
    void itemRemoved(int);
    void finished();
    void updateClientListWidget(int);
    void newClientAdded(QString clientId, int index);
    void prepPacketToPublish(int, QByteArray);
    void sendPublishTopic(QString topic);
public slots:
    void slotReadyRead(int index);
    void slotDisconnect(int index);
    void slotClientConnected(int index);
    void slotPublishPacket(int);
    void itemHasBeenRemoved();
public:
    MQTTBridgeServer(QObject *parent = nullptr);
    void startServer(QHostAddress hostAddress, int port);
    void adjustClientList(int index);
    QObject *parent = nullptr;
    QList<ClientSock*> Clients;
    QSignalMapper *map;
    QSignalMapper *clientSockDisconnectMap;
    QSignalMapper *clientConnectedMap;
    void setBrokerInfo(QString p_brokerIP, int p_brokerPort);
private:
    QString brokerIP;
    int brokerPort;
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // MQTTBRIDGESERVER_H
