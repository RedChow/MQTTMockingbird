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
#include "mqttbridgeserver.h"
#include <QDebug>

#include "mqttpacket.h"

MQTTBridgeServer::MQTTBridgeServer(QObject *parent) :
    QTcpServer(parent),
    parent(parent)
{
    map = new QSignalMapper(this);
    connect(map, SIGNAL(mapped(int)), this, SLOT(slotReadyRead(int)));

    clientSockDisconnectMap = new QSignalMapper(this);
    connect(clientSockDisconnectMap, SIGNAL(mapped(int)), this, SLOT(slotDisconnect(int)));

    clientConnectedMap = new QSignalMapper(this);
    connect(clientConnectedMap, SIGNAL(mapped(int)), this, SLOT(slotClientConnected(int)));

    //default broker and port
    brokerIP = "0.0.0.0";
    brokerPort = 1884;
}

void MQTTBridgeServer::setBrokerInfo(QString p_brokerIP, int p_brokerPort) {
    brokerIP = p_brokerIP;
    brokerPort = p_brokerPort;
}

void MQTTBridgeServer::startServer(QHostAddress hostAddress, int port) {
    if (this->listen(hostAddress, port)) {
        emit serverConnected();
    }

}

void MQTTBridgeServer::slotClientConnected(int index) {
    emit newClientAdded(Clients[index]->clientId, index);
}

void MQTTBridgeServer::incomingConnection(qintptr socketDescriptor) {
    ClientSock *newSocket = new ClientSock();
    newSocket->setBrokerInfo(brokerIP, brokerPort);
    newSocket->connectBroker();
    newSocket->setSocketDescriptor(socketDescriptor);
    Clients.append(newSocket);
    map->setMapping(Clients.last(), Clients.length()-1);
    connect(Clients.last(), SIGNAL(readyRead()), map, SLOT(map()));
    clientSockDisconnectMap->setMapping(Clients.last(), Clients.length()-1);
    connect(Clients.last(), SIGNAL(disconnected()), clientSockDisconnectMap, SLOT(map()));
    clientConnectedMap->setMapping(Clients.last(), Clients.length()-1);
    connect(Clients.last(), SIGNAL(clientConnected()), clientConnectedMap, SLOT(map()));
}

void MQTTBridgeServer::slotDisconnect(int index) {
    emit updateClientListWidget(index);
}

void MQTTBridgeServer::adjustClientList(int index) {
    pauseAccepting();
    Clients.removeAt(index);
    int i;
    for (i = 0; i < Clients.size(); i++) {
        map->setMapping(Clients.at(i), i);
        connect(Clients.at(i), SIGNAL(readyRead()), map, SLOT(map()));
        clientSockDisconnectMap->setMapping(Clients.at(i), i);
        connect(Clients.at(i), SIGNAL(disconnected()), clientSockDisconnectMap, SLOT(map()));
        clientConnectedMap->setMapping(Clients.last(), Clients.length()-1);
        connect(Clients.last(), SIGNAL(clientConnected()), clientConnectedMap, SLOT(map()));
    }
}

void MQTTBridgeServer::itemHasBeenRemoved() {
    resumeAccepting();
}

void MQTTBridgeServer::slotReadyRead(int index)
{
    QByteArray message = Clients.at(index)->readAll();
    if (message.size() > 0) {
        Clients.at(index)->processPacket(message);
        emit updateClientListWidget(index);
    }
}


