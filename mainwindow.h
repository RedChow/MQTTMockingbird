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
