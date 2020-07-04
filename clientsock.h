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
