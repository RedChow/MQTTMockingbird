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
    QTcpSocket *brokerSocket;
    QString brokerIP;
    int brokerPort;
    void setBrokerInfo(QString p_brokerIP, int p_brokerPort);
    void connectBroker();
    TopicTrie *topicTrie;
    TopicTrie *reverseTopicTrie;
    QString clientId;
    QString username;
    QString password;
    int mqttVersion;
    bool willRetain;
    int qos;
    bool willFlag;
    bool cleanSession;
    QString lastContact;
    QString lastPacketType;

    void processPacket(QByteArray&, bool serverToClient=false);
    void setClientId(QString);
signals:
    void clientConnected();
    void publishPacket();

public slots:
    void writeToSocket(QByteArray p_packet, bool serverToClient=false);
    void updateClientName(QString);
    void test(QAbstractSocket::SocketState);
    void readyReadPacket();
};

#endif // CLIENTSOCK_H
