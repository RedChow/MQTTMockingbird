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

