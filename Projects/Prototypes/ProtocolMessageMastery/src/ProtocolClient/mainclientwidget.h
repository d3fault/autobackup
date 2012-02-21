#ifndef MAINCLIENTWIDGET_H
#define MAINCLIENTWIDGET_H

#include <QtGui/QWidget>

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QThread>

#include "protocolclient.h"

class mainClientWidget : public QWidget
{
    Q_OBJECT

public:
    mainClientWidget(QWidget *parent = 0);
    ~mainClientWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartStopButton;
    QThread *m_ClientThread;
    //hello old friends

    ProtocolClient *m_Client;
};

#endif // MAINCLIENTWIDGET_H
