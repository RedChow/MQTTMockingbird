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
