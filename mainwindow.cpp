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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtNetwork/QHostAddress>
#include <QDialog>
#include <QtWebSockets/QtWebSockets>
#include <QUuid>
#include <QMessageBox>
#include <memory>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonStartClientBridge, &QPushButton::clicked, this, &MainWindow::startMQTTBridgeServer);
    connect(ui->pushButtonStopClientBridge, &QPushButton::clicked, this, &MainWindow::stopMQTTBridgeServer);
    connect(ui->pushButtonAddTopicMapping, &QPushButton::clicked, this, &MainWindow::openTopicMappingDialog);
    connect(ui->pushButtonBrokerConnect, &QPushButton::clicked, this, &MainWindow::setClientSockBrokerInfo);
    QStringList tableHeaders;
    tableHeaders << "Device Id" << "Incoming Topic" << "Outgoing Topic";
    ui->tableWidgetTopicMappings->setHorizontalHeaderLabels(tableHeaders);

    //TODO: Remove the next line
    addNewTopicToTrie(QString("this/is/a/{0}@{1}/badtopic"), QString("this/is/a/{0}"), QString("mqttx_c1a70902"));

    connect(ui->tableWidgetTopicMappings, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableHasBeenEdited(QTableWidgetItem*)));
    connect(ui->tableWidgetTopicMappings, SIGNAL(itemSelectionChanged()), this, SLOT(tableItemSelectionChanged()));
}

void MainWindow::tableItemSelectionChanged() {
    QList<QTableWidgetItem*> selectedItems = ui->tableWidgetTopicMappings->selectedItems();
    if (selectedItems.size() > 0) {
        if (selectedItems[0]->column() == 0) {
            previousClientId = selectedItems[0]->text();
        }
    }
}

void MainWindow::tableHasBeenEdited(QTableWidgetItem *item) {
    QString tempClientId = item->text();
    if (item->column() != 0) {
        tempClientId = ui->tableWidgetTopicMappings->itemAt(item->row(), 0)->text();
    }
    if (item->column() == 0) {
        TopicTrie *mapFromClientTopicTrie = mapFromClient[previousClientId];
        TopicTrie *mapToClientTopicTrie = mapToClient[previousClientId];
        mapFromClient[item->text()] = mapFromClientTopicTrie;
        mapToClient[item->text()] = mapToClientTopicTrie;
    }
    if (item->column() == 1) {
        //mapping rule update
        TopicTrie *tempTopicTrie = mapFromClient[tempClientId];
        tempTopicTrie->setMappingRule(item->text());
    }
    else if(item->column() == 2) {
        //remap rule
        TopicTrie *tempTopicTrie = mapFromClient[tempClientId];
        tempTopicTrie->setRemapRule(item->text());
    }
    updateClientListWidgetMain();
}

void MainWindow::openTopicMappingDialog() {
    QList<QString> clientIds;
    for (auto client: mqttBridgeServer->Clients) {
        clientIds << client->clientId;
    }
    topicMappingDialog = new TopicMappingDialog(this);
    topicMappingDialog->setClientIds(clientIds);
    connect(topicMappingDialog, SIGNAL(addTopicToTrie(QString, QString, QString)), this, SLOT(addNewTopicToTrie(QString, QString, QString)));
    topicMappingDialog->show();
}

void MainWindow::addNewTopicToTrie(QString incomingTopicRule, QString mappedTopicRule, QString clientId) {
    if (mapFromClient[clientId] != nullptr) {
        QMessageBox messageBox;
        messageBox.setText("Mapping Rule for client already exists. Cannot add another topic for this client.");
        messageBox.exec();
        return;
    }
    disconnect(ui->tableWidgetTopicMappings, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableHasBeenEdited(QTableWidgetItem*)));
    TopicTrie *topicTrieClient = new TopicTrie();
    topicTrieClient->setMappingRule(incomingTopicRule);
    topicTrieClient->setRemapRule(mappedTopicRule);

    TopicTrie *reverseTopicTrieClient = new TopicTrie();

    mapFromClient[clientId] = topicTrieClient;
    mapToClient[clientId] = reverseTopicTrieClient;
    ui->tableWidgetTopicMappings->insertRow(ui->tableWidgetTopicMappings->rowCount());
    ui->tableWidgetTopicMappings->setItem(ui->tableWidgetTopicMappings->rowCount()-1, 0, new QTableWidgetItem(clientId));
    ui->tableWidgetTopicMappings->setItem(ui->tableWidgetTopicMappings->rowCount()-1, 1, new QTableWidgetItem(incomingTopicRule));
    ui->tableWidgetTopicMappings->setItem(ui->tableWidgetTopicMappings->rowCount()-1, 2, new QTableWidgetItem(mappedTopicRule));

    updateClientListWidgetMain();
    connect(ui->tableWidgetTopicMappings, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableHasBeenEdited(QTableWidgetItem*)));
}

void MainWindow::startMQTTBridgeServer() {
    connect(this->mqttBridgeServer, SIGNAL(serverConnected()), this, SLOT(bridgeIsConnected()));
    this->mqttBridgeServer->startServer(QHostAddress(ui->lineEditIP->text()), ui->spinBoxPort->value());
    connect(this->mqttBridgeServer, SIGNAL(itemAdded()), this, SLOT(updateClientListWidgetMain()));
    connect(this->mqttBridgeServer, SIGNAL(itemRemoved(int)), this, SLOT(updateClientListWidgetMain()));
    connect(this->mqttBridgeServer, SIGNAL(updateClientListWidget(int)), this, SLOT(refreshList(int)));
    connect(this, SIGNAL(itemRemovedMain()), this->mqttBridgeServer, SLOT(itemHasBeenRemoved()));
    connect(this->mqttBridgeServer, SIGNAL(newClientAdded(QString, int)), this, SLOT(addNewClient(QString, int)));
}

void MainWindow::addNewClient(QString clientId, int index) {
    Q_UNUSED(index);
    int i;
    int indexToDelete{-1};
    for (i = 0; i < mqttBridgeServer->Clients.size() - 1; i++) {
        if (mqttBridgeServer->Clients.at(i)->clientId == clientId) {
            indexToDelete = i;
            break;
        }
    }
    if (indexToDelete >= 0) {
        mqttBridgeServer->adjustClientList(indexToDelete);
    }
    updateClientListWidgetMain();
}

void MainWindow::stopMQTTBridgeServer() {
    this->mqttBridgeServer->close();
    ui->labelStatus->setStyleSheet("QLabel {background-color: red; color: white;}");
    ui->labelStatus->setText("Not Listening");
}

void MainWindow::bridgeIsConnected() {
    ui->labelStatus->setStyleSheet("QLabel {background-color: green; color: white;}");
    ui->labelStatus->setText("Listening");
}

void MainWindow::setClientSockBrokerInfo() {
    mqttBridgeServer->setBrokerInfo(ui->lineEditBrokerIP->text(), ui->spinBoxBrokerPort->value());
    ui->pushButtonStartClientBridge->setEnabled(true);
}

void MainWindow::updateClientListWidgetMain() {
    ui->listWidgetClients->clear();
    int i;
    for (i = 0; i < mqttBridgeServer->Clients.size(); i++) {
        ClientInfoWidget *clientInfoWidget = new ClientInfoWidget();
        clientInfoWidget->setClientSock(*mqttBridgeServer->Clients.at(i));
        mqttBridgeServer->Clients.at(i)->topicTrie = mapFromClient[mqttBridgeServer->Clients.at(i)->clientId];
        mqttBridgeServer->Clients.at(i)->reverseTopicTrie = mapToClient[mqttBridgeServer->Clients.at(i)->clientId];
        QListWidgetItem *lwi = new QListWidgetItem();
        ui->listWidgetClients->addItem(lwi);
        ui->listWidgetClients->setItemWidget(lwi, clientInfoWidget);
        lwi->setSizeHint(clientInfoWidget->sizeHint());
    }
    if (mqttBridgeServer->isListening()) {
        mqttBridgeServer->resumeAccepting();
     }
}

void MainWindow::refreshList(int row) {
    if (row < ui->listWidgetClients->count()) {
        QListWidgetItem *item = ui->listWidgetClients->item(row);
        ClientInfoWidget *clientInfoWidget = dynamic_cast<ClientInfoWidget*>(ui->listWidgetClients->itemWidget(item));
        clientInfoWidget->updateMyself();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mqttBridgeServer;
}

