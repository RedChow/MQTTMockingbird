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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <QTableWidgetItem>
#include "mqttbridgeserver.h"
#include "clientinfowidget.h"
#include "topicmappingdialog.h"
#include "topictrie.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    TopicMappingDialog *topicMappingDialog;
    void startMQTTBridgeServer();
    void stopMQTTBridgeServer();
    void openTopicMappingDialog();
    TopicTrie *topicTrie;
    TopicTrie *reverseTopicTrie;
    QHash<QString, TopicTrie*> mapFromClient;
    QHash<QString, TopicTrie*> mapToClient;
    void setClientSockBrokerInfo();
signals:
    void sendNewTopic(QString);
    void itemRemovedMain();
public slots:
    void bridgeIsConnected();
    void updateClientListWidgetMain();
    void refreshList(int);
    void addNewClient(QString clientId, int index);
    void addNewTopicToTrie(QString incomingTopicRule, QString mappedTopicRule, QString clientId);
    void tableHasBeenEdited(QTableWidgetItem *item);
    void tableItemSelectionChanged();
private:
    Ui::MainWindow *ui;
    MQTTBridgeServer* mqttBridgeServer = new MQTTBridgeServer(this);
    QString previousClientId;
};
#endif // MAINWINDOW_H
