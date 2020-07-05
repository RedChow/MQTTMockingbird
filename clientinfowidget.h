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

#ifndef CLIENTINFOWIDGET_H
#define CLIENTINFOWIDGET_H

#include <QWidget>
#include "clientsock.h"

namespace Ui {
class ClientInfoWidget;
}

class ClientInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoWidget(QWidget *parent = nullptr);
    ~ClientInfoWidget();
    ClientSock *clientSock;
    void setClientSock(ClientSock&);
    void updateMyself();
private:
    Ui::ClientInfoWidget *ui;
};

#endif // CLIENTINFOWIDGET_H
