/*
 * Copyright (C) 2020 brian DOT l DOT miller DOT ttu AT gmail DOT com
 * This file is part of MQTTMockingbird.
 *
 * This program comes with ABSOLUTELY NO WARRANTY
 * MQTTMockingbird is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MQTTMockingbird is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MQTTMockingbird.  If not, see <https://www.gnu.org/licenses/.
 */
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
    void updateClientListWidget(int);
    void newClientAdded(QString clientId, int index);
    void prepPacketToPublish(int, QByteArray);
    void sendPublishTopic(QString topic);
public slots:
    void slotReadyRead(int index);
    void slotDisconnect(int index);
    void slotClientConnected(int index);
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
