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
#ifndef CLIENTSOCK_H
#define CLIENTSOCK_H
#include <QTcpSocket>
#include <QObject>
#include "mqttpacket.h"
#include "topictrie.h"
/*
 * 	ClientSock will call ProcessMessage to determine what to do with the message received
 */

class ClientSock : public QTcpSocket
{
    Q_OBJECT
public:
    ClientSock();
    QString clientId;
    QString lastContact;
    QString lastPacketType;
    TopicTrie *topicTrie;
    TopicTrie *reverseTopicTrie;
    void setBrokerInfo(QString p_brokerIP, int p_brokerPort);
    void connectBroker();
    void processPacket(QByteArray&, bool serverToClient=false);
    void setClientId(QString);
private:
    QTcpSocket *brokerSocket;
    QString brokerIP;
    int brokerPort;
    QString username;
    QString password;
    int mqttVersion;
    bool willRetain;
    int qos;
    bool willFlag;
    bool cleanSession;
signals:
    void clientConnected();
public slots:
    void writeToSocket(QByteArray p_packet, bool serverToClient=false);
    void updateClientName(QString);
    void readyReadPacket();
};

#endif // CLIENTSOCK_H
