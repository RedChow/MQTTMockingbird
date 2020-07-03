#ifndef TOPICMAPPINGDIALOG_H
#define TOPICMAPPINGDIALOG_H

#include <QDialog>

namespace Ui {
class TopicMappingDialog;
}

class TopicMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TopicMappingDialog(QWidget *parent = nullptr);
    ~TopicMappingDialog();
    void setNewTopic();
    void sendAddTopicToTrie();
    QList<QString> clientIds;
    void setClientIds(QList<QString> p_clientIds);
    void fillComboBox();
signals:
    void addTopicToTrie(QString incomingTopicRule, QString mappedTopicRule, QString clientId);

private:
    Ui::TopicMappingDialog *ui;
};

#endif // TOPICMAPPINGDIALOG_H
