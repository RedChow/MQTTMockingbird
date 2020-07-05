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
