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
#include "mqttpacket.h"
#include <QDebug>

enum ConnackReturnCodes {
    ACCEPTED = 1,
    UNACCEPTED_PROTOCOL,
    IDENTIFIER_REJECTED,
    SERVER_UNAVAILABLE,
    BAD_USERNAME_PASSWORD,
    NOT_AUTHORIZED
};

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

MQTTPacketv2::MQTTPacketv2(QByteArray &mqttPacket) :
    packet(mqttPacket)
{

}

quint8 MQTTPacketv2::getCommand(char c) {
    return (c >> 4);
}

quint8 MQTTPacketv2::getQoS(char c) {
    return (c & 0x06) >> 1;
}

quint8 MQTTPacketv2::getRetain(char c) {
    return (c & 0x01);
}

quint8 MQTTPacketv2::getDuplicate(char c) {
    return (c & 0x08) >> 3;
}

void MQTTPacketv2::setFirstByte(quint8 command, quint8 retain, quint8 qos, quint8 duplicate) {
    //packet[0] = (command << 4) |  (retain | qos << 1 | duplicate << 3);
    packet.push_back(quint8 ((command << 4) |  (retain | qos << 1 | duplicate << 3)));
    //qDebug() << packet.toHex() << quint8 ((command << 4) |  (retain | qos << 1 | duplicate << 3));
}

quint8 MQTTPacketv2::popNumberByte() {
    quint8 temp = static_cast<quint8>(packet.at(0));
    packet.remove(0, 1);
    remainingLength--;
    return temp;
}

int MQTTPacketv2::getRemainingLength() {
    int multiplier{1};
    quint8 b{0};
    int temp{0};
    while (multiplier < 128*128*128) {
        //pop next byte
        //b = static_cast<quint8>(packet[0]);
        b = static_cast<quint8>(packet.at(0));
        packet.remove(0, 1);
        temp += (b & 127)*multiplier;
        if ( (b&128) == 0) {
            return temp;
        }
        multiplier *= 128;
    }
    return -1;
}

void MQTTPacketv2::setRemainingLength(int length) {
    int bytes = 128*128*128;
    quint8 d{0};
    while (bytes >= 1) {
        d = length/bytes;
        if (d != 0) {
            //packet.append(QString("%1").arg(d, 2, 16));
            packet.append(d);
            qDebug() << "MQTTv2:: -----------> " << d << " " << length;
            length = length - d*bytes;
        }
        bytes /= 128;
    }
}

void MQTTPacketv2::getFixedHeader() {
    //firstByte = packet[0];
    firstByte = packet.at(0);
    command = static_cast<quint8> (firstByte >> 4);
    //can get retain, qos, and duplicate with their respective get methods later if it's a publish packet
    packet.remove(0, 1);
    remainingLength = getRemainingLength();
}

quint16 MQTTPacketv2::getLength() {
    //almost all string fields are preceeded by a two byte length = (byte 1)*16*16 + (byte 2)
    quint8 byteOne = static_cast<quint8>(packet.at(0));
    quint8 byteTwo = static_cast<quint8>(packet.at(1));
    quint16 length = (byteOne << 8) | byteTwo;
    packet.remove(0, 2);
    remainingLength -= 2;
    return length;
}

void MQTTPacketv2::setLength(quint16 length) {
    //reverse from the getLength method above
    quint8 byteOne = (length & 0xff00) >> 8; //& 0xff00 captures everything in bits 8-15 starting at bit 0, shift left 8 bits so that it becomes a quint8
                                            // length can't be more than two bytes
    quint8 byteTwo = (length & 0x00ff);      //& 0x00ff captures everything in bits 0-7 starting at bit 0
    packet.append(byteOne).append(byteTwo);
}

QString MQTTPacketv2::getString(int length) {
    QString string = QString::fromUtf8(packet.left(length));
    packet.remove(0, length);
    remainingLength -= length;
    return string;
}

void MQTTPacketv2::parseConnectVariableHeader(QString &p_clientId) {
    //We will build a returning packet as we parse this packet
    setFirstByte(CONNACK);
    //bytes 1 and 2 are protocol name length = (byte 1)*16*16 + byte 2
    quint16 length = getLength();
    QString protocol = getString(length);
    if (protocol != QString("MQTT")) {
        //need to remove rest of the packet and return a disconnect packet
        //return;
        //we do nothing for now and pass the packet to the broker
    }
    quint8 protocolLevel = popNumberByte();
    if (protocolLevel > 5) {
        //need to remove the rest of the packet and return a disconnect packet
        //return;
        //we do nothing for now and pass the packet to the broker
    }
    //connect flags
    //byte =   7 	|	6	| 	5	 	|	4 & 3 	| 	2 		|	1 		| 0
    //     username | pass	|will retain| will qos	|will flag	| clean sess| reserved
    quint8 connectFlags = popNumberByte();
    Q_UNUSED(connectFlags);

    //keep alive
    quint16 keepAlive = getLength();
    Q_UNUSED(keepAlive);

    //rest is the payload
    //1 client id
    //IF client id is 0 bytes and clean session is 0, must send error code 0x02 and disconnect
    quint16 clientIdLength = getLength();
    p_clientId = getString(clientIdLength);

    qDebug() << "MQTTv2: " << p_clientId;
    //2 will topic

    //3 will message

    //4 username

    //5 password
}

void MQTTPacketv2::parsePublishVariableHeader() {
    //topic length is first two bytes
    //get topic
    //remaingPacketLength - topicLength is payload
    quint16 topicLength = getLength();
    QString topic = getString(topicLength);
    QString payload = getString(remainingLength);
    info["topic"] = topic;
    info["payload"] = payload;
}

void MQTTPacketv2::setPublishVariableHeader(QString topic, QString payload) {
    //we set the two byte length for the topic; payload is remaingLength - topicLength
    setLength(topic.size());
    packet.append(topic).append(payload);
}

QMap<QString, quint8> MQTTPacketv2::parseSubscribeVariableHeader(quint16 &packetIdentifier) {
    //2 byte length give topic length, next byte is QoS
    QMap<QString, quint8> topicQoSMap;
    QString tempString = "";
    quint8 tempQoS{0};
    qDebug() << "MQTTPacketv2:: parseSubscribeVariableHeader " << packet << " " << remainingLength;
    packetIdentifier = getLength();
    while (remainingLength > 0) {
        quint16 length = getLength();
        tempString = getString(length);
        tempQoS = popNumberByte();
        topicQoSMap[tempString] = tempQoS;
    }
    return topicQoSMap;
}

void MQTTPacketv2::setSubscribeVariableHeader(quint16 &packetIdentifier, QMap<QString, quint8> subscribeList) {
    //set packet identifier
    setLength(packetIdentifier);
    //for each key, value pair we write Length, String, and then QoS
    QMapIterator<QString, quint8> iterator(subscribeList);
    while (iterator.hasNext()) {
        iterator.next();
        setLength(iterator.key().size());
        packet.append(iterator.key());
        packet.append(iterator.value());
    }
}

void MQTTPacketv2::printPacket() {
    qDebug() << "MQTTv2:: " << packet.toHex();
    qDebug() << "MQTTv2:: " << packet;
}

QByteArray MQTTPacketv2::returnPacket() {
    return packet;
}

/*Control Packet Summary
 *Variable headers are required for:
 *  ALL BUT CONNACK, PINGREQ, PINGRESP, DISCONNECT
 * Payload required for:
 *  CONNECT, PUBLISH (OPTIONAL), SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK
 *
 *
 * MSB = x -> (x & 0xFF00) >> 8
 * LSB = x -> (x & 0x00FF)
 * to go other way:
 * 		(MSB << 8) | LSB
 */
