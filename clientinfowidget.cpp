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
#include "clientinfowidget.h"
#include "ui_clientinfowidget.h"

ClientInfoWidget::ClientInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientInfoWidget)
{
    ui->setupUi(this);
}

void ClientInfoWidget::setClientSock(ClientSock &p_clientSock) {
    clientSock = &p_clientSock;
    updateMyself();
}

void ClientInfoWidget::updateMyself() {
    ui->labelClientId->setText(clientSock->clientId);
    ui->labelLastContact->setText(clientSock->lastContact);
    ui->labelLastPacketType->setText(clientSock->lastPacketType);
    if (clientSock->state() == 3) {
        ui->labelState->setStyleSheet("QLabel {background-color: green; color: white;}");
        ui->labelState->setText("Conn");
    }
    else if(clientSock->state() == 0) {
        ui->labelState->setStyleSheet("QLabel {background-color: red; color: white;}");
        ui->labelState->setText("Dis");
    }
    else {
        ui->labelState->setStyleSheet("QLabel {background-color: yellow; color: black;}");
        ui->labelState->setText("Und");
    }

    if (clientSock->state() == 3) {
        ui->labelBrokerStatus->setStyleSheet("QLabel {background-color: green; color: white;}");
        ui->labelBrokerStatus->setText("Conn");
    } else if (clientSock->state() == 0) {
        ui->labelBrokerStatus->setStyleSheet("QLabel {background-color: red; color: white;}");
        ui->labelBrokerStatus->setText("Disconnected");
    } else {
        ui->labelBrokerStatus->setStyleSheet("QLabel {background-color: yellow; color: white;}");
        ui->labelBrokerStatus->setText("Und");
    }
}

ClientInfoWidget::~ClientInfoWidget()
{
    delete ui;
}
