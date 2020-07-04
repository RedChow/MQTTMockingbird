#include "topicmappingdialog.h"
#include "ui_topicmappingdialog.h"
#include <QMessageBox>
#include "diff.h"

TopicMappingDialog::TopicMappingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TopicMappingDialog)
{
    ui->setupUi(this);
    connect(ui->pushButtonUpdateNewTopic, &QPushButton::clicked, this, &TopicMappingDialog::setNewTopic);
    connect(ui->pushButtonAddTopicMapping, &QPushButton::clicked, this, &TopicMappingDialog::sendAddTopicToTrie);
}

void TopicMappingDialog::setClientIds(QList<QString> p_clientIds) {
    for (auto clientId: p_clientIds) {
        ui->comboBoxClientId->addItem(clientId, clientId);
    }
}

void TopicMappingDialog::setNewTopic() {
    Diff diffTopic(ui->lineEditIncomingTopicRule->text(), ui->lineEditTestIncoming->text(), ui->lineEditRemapTopicRule->text());
    QString newTopic = diffTopic.getMappedTopic();
    ui->lineEditNewTopic->setText(newTopic);
}

void TopicMappingDialog::sendAddTopicToTrie() {
    if (ui->comboBoxClientId->currentText().isEmpty()) {
        QMessageBox messageBox;
        messageBox.setText("Client Id cannot be empty. Please enter a Client Id.");
        messageBox.exec();
        return;
    }
    emit addTopicToTrie(ui->lineEditIncomingTopicRule->text(), ui->lineEditRemapTopicRule->text(), ui->comboBoxClientId->currentText());
    close();
}

TopicMappingDialog::~TopicMappingDialog()
{
    delete ui;
}
