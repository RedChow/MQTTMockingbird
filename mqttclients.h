#ifndef MQTTCLIENTS_H
#define MQTTCLIENTS_H
#include "clientsock.h"


class MQTTClients
{
public:
    MQTTClients();
    QList<ClientSock*> Clients;
};

#endif // MQTTCLIENTS_H
