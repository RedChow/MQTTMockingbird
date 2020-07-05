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
#include "clientsock.h"
#include <QDateTime>
#include <QDebug>
#include <memory>
#include "diff.h"

enum ControlHeader {
    CONNECT = 1,
    CONNACK,
    PUBLISH,
    PUBACK,
    PUBREC,
    PUBREL,
    PUBCOMP,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT
};

QStringList PacketTypes = {"CONNECT", "CONNACK", "PUBLISH", "PUBACK", "PUBREC", "PUBREL", "PUBCOMP", "SUBSCRIBE", "SUBACK", "UNSUBSCRIBE", "UNSUBACK", "PINGREQ", "PINGRESP", "DISCONNECT"};

enum PublishFlags {

};

enum ConnackReturnCodes {
    ACCEPTED = 1,
    UNACCEPTED_PROTOCOL,
    IDENTIFIER_REJECTED,
    SERVER_UNAVAILABLE,
    BAD_USERNAME_PASSWORD,
    NOT_AUTHORIZED
};

ClientSock::ClientSock()
{

}

void ClientSock::setBrokerInfo(QString p_brokerIP, int p_brokerPort) {
    brokerIP = p_brokerIP;
    brokerPort = p_brokerPort;
}

void ClientSock::connectBroker() {
    brokerSocket = new QTcpSocket();
    brokerSocket->connectToHost(brokerIP, brokerPort);
    connect(brokerSocket, SIGNAL(readyRead()), this, SLOT(readyReadPacket()));
}

void ClientSock::setClientId(QString p_ClientId) {
    clientId = p_ClientId;
}

void ClientSock::processPacket(QByteArray &qba, bool serverToClient) {
    MQTTPacketv2 *packetv2 = new MQTTPacketv2(qba);
    packetv2->getFixedHeader();
    lastPacketType = PacketTypes[packetv2->command - 1];
    switch (packetv2->command) {
        case CONNECT:
            //always client to server
            brokerSocket->write(qba);
            packetv2->parseConnectVariableHeader(clientId);
            emit clientConnected();
            break;
        case CONNACK:
            //always server to client
            write(qba);
            break;
        case PUBLISH:
            {
                packetv2->parsePublishVariableHeader();
                /*
                 * 	Apply diffTopic to incoming topic
                 * 		topicTrie->mappingRule: holds map from client to server
                 * 		topicTrie->remapRule: holds map from server to client
                 */
                if (topicTrie == nullptr) {
                     writeToSocket(qba, serverToClient);
                     break;
                }
                QString newTopic{""};
                //search first to see if topic already exists, if not, add it
                if (!topicTrie->search(packetv2->info["topic"].toString())) {
                    if (!serverToClient) {
                        Diff diffTopic(topicTrie->mappingRule, packetv2->info["topic"].toString(), topicTrie->remapRule);
                        newTopic = diffTopic.getMappedTopic();
                        //TODO: how to tell if getMappedTopic fails; fall back to topic if it fails
                    }
                    topicTrie->insertNewTopic(packetv2->info["topic"].toString(), newTopic);
                    reverseTopicTrie->insertNewTopic(newTopic, packetv2->info["topic"].toString());
                }
                QString mapToTopic = topicTrie->getMapToTopic(packetv2->info["topic"].toString());

                if (serverToClient) {
                    mapToTopic = reverseTopicTrie->getMapToTopic(packetv2->info["topic"].toString());
                }
                if (!mapToTopic.isEmpty()) {
                    //If getMapToTopic is not empty, the topic is mapped to a new topic. Thus we create a new header and
                    //need to preserve flags
                    packetv2->setFirstByte(PUBLISH);
                    packetv2->setRemainingLength(2 + mapToTopic.size() + packetv2->info["payload"].toString().size());
                    packetv2->setPublishVariableHeader(mapToTopic, packetv2->info["payload"].toString());
                    packetv2->printPacket();
                    writeToSocket(packetv2->returnPacket(), serverToClient);
                }
                else {
                    writeToSocket(qba, serverToClient);
                }
            }
            break;
        case PUBACK:
            //received from broker after PUBLISH with QoS = 1
            writeToSocket(qba, serverToClient);
            break;
        case PUBREC:
            //recieved from broker after PUBLISH with QoS = 2
            writeToSocket(qba, serverToClient);
            break;
        case PUBREL:
            //response to a PUBREC
            writeToSocket(qba, serverToClient);
            break;
        case PUBCOMP:
            //reponse to PUBREL
            writeToSocket(qba, serverToClient);
            break;
        case SUBSCRIBE:
            //always client to server
            {
                //test if there's a topi trie
                if (topicTrie == nullptr) {
                     writeToSocket(qba, serverToClient);
                     break;
                }
                quint16 packetIdentifier;
                QMap<QString, quint8> subscribeInfos =  packetv2->parseSubscribeVariableHeader(packetIdentifier);
                QMapIterator<QString, quint8> iterator(subscribeInfos);
                QMap<QString, quint8> newSubscribeInfos;
                quint16 remainingSize = 2; //start with 2 for the packetIdentifier
                QString tempTopic = QString("");
                QString newTopic = QString("");
                while (iterator.hasNext()) {
                    iterator.next();
                    tempTopic = topicTrie->getMapToTopic(iterator.key());
                    remainingSize += 3; //1 for qos and 2 for the length
                    if (!tempTopic.isEmpty()) {
                        remainingSize += tempTopic.size();
                        newSubscribeInfos[tempTopic] = iterator.value();
                    }
                    else {
                        //if no topic has been mapped yet, we create one
                        Diff diffTopic(topicTrie->mappingRule, iterator.key(),topicTrie->remapRule);
                        newTopic = diffTopic.getMappedTopic();
                        //TODO: how to tell if getMappedTopic fails; fall back to topic if it fails
                        topicTrie->insertNewTopic(packetv2->info["topic"].toString(), newTopic);
                        reverseTopicTrie->insertNewTopic(newTopic, iterator.key());

                        remainingSize += newTopic.size();
                        newSubscribeInfos[newTopic] = iterator.value();
                    }
                }
                packetv2->setFirstByte(SUBSCRIBE, 0, 1, 0);
                packetv2->setRemainingLength(remainingSize);
                packetv2->setSubscribeVariableHeader(packetIdentifier, newSubscribeInfos);
                //packetv2->printPacket();
                brokerSocket->write(packetv2->returnPacket());
            }
            break;
        case SUBACK:
            //alwats server to client
            write(qba);
            break;
        case UNSUBSCRIBE:
            //always client to server
            //will be similar to SUBSCRIBE, except there is no qos field
            //TODO: Implement UNSUBSCRIBE
            break;
        case UNSUBACK:
            //always server to client
            write(qba);
            break;
        case PINGREQ:
            //always client to server
            brokerSocket->write(qba);
            break;
        case PINGRESP:
            //always server to client
            write(qba);
            break;
        case DISCONNECT:
            //always client to server
            brokerSocket->write(qba);
            //emit clientConnected();
            break;
        default:
            break;
    }
    lastContact = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

void ClientSock::writeToSocket(QByteArray p_packet, bool serverToClient) {
    if (!serverToClient) {
        brokerSocket->write(p_packet);
    }
    else {
        write(p_packet);
    }
}

void ClientSock::updateClientName(QString p_ClientName) {
    clientId = p_ClientName;
}

void ClientSock::readyReadPacket() {
    QByteArray qba = brokerSocket->readAll();
    if (qba.size() > 0) {
        processPacket(qba, true);
    }
}
