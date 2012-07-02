#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QThread>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "rpcserverbusinessimpl.h"
#include "rpcclientshelperanddeliveryacceptorandnetwork.h"

class mainWidget : public QWidget
{
    Q_OBJECT
public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();
private:
    QThread *m_BusinessThread;
    RpcServerBusinessImpl *m_Business;

    QThread *m_NetworkThread;
    RpcClientsHelperAndDeliveryAcceptorAndNetwork *m_ClientsHelper;

    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_SimulateActionButton;
};

#endif // MAINWIDGET_H
