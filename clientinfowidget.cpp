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
