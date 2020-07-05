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
#ifndef MQTTPACKET_H
#define MQTTPACKET_H
#include <QtCore>
#include <QObject>
#include <QHash>

/*
 *  This is MQTTPacketv2 as the first version was an absolute horrid mess; this is only <i>slightly</li> better.
 */

class MQTTPacketv2: public QObject
{
    Q_OBJECT
public:
    MQTTPacketv2(QByteArray &mqttPacket);
    quint8 command;
    int remainingLength;
    quint8 getCommand(char c);
    quint8 getQoS(char c);
    quint8 getRetain(char c);
    quint8 getDuplicate(char c);
    int getRemainingLength();
    void setCommand(int command);
    void setQoS(int qos);
    void setRetain(int retain);
    void setDuplicate(int duplicate);
    void setFirstByte(quint8 command, quint8 retain = 0, quint8 qos = 0, quint8 duplicate = 0);
    void setRemainingLength(int length);

    quint8 popNumberByte();
    quint16 getLength();
    void setLength(quint16 length);
    QString getString(int length);
    void getFixedHeader();
    void parseConnectVariableHeader(QString &p_clientId);
    void parsePublishVariableHeader();
    void setPublishVariableHeader(QString topic, QString payload);
    QMap<QString, quint8> parseSubscribeVariableHeader(quint16 &packetIdentifier);
    void setSubscribeVariableHeader(quint16 &packetIdentifier, QMap<QString, quint8> subscribeList);
    void printPacket();
    QByteArray returnPacket();
    QHash<QString, QVariant> info;
private:
    QByteArray packet;
    unsigned char firstByte;
};


#endif // MQTTPACKET_H

