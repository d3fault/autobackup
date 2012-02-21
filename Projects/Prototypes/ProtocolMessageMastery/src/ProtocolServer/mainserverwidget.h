#ifndef MAINSERVERWIDGET_H
#define MAINSERVERWIDGET_H

#include <QtGui/QWidget>

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QThread>

#include "protocolserver.h"

class mainServerWidget : public QWidget
{
    Q_OBJECT

public:
    mainServerWidget(QWidget *parent = 0);
    ~mainServerWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartStopButton;
    QThread *m_ServerThread;
    //hello old friends

    ProtocolServer *m_Server;
};

#endif // MAINSERVERWIDGET_H
