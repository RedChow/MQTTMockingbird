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
    void setClientIds(QList<QString> p_clientIds);
    void fillComboBox();
signals:
    void addTopicToTrie(QString incomingTopicRule, QString mappedTopicRule, QString clientId);

private:
    Ui::TopicMappingDialog *ui;
};

#endif // TOPICMAPPINGDIALOG_H
