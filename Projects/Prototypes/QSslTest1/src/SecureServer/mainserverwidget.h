#ifndef MAINSERVERWIDGET_H
#define MAINSERVERWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QThread>
#include <QPlainTextEdit>
#include <QPushButton>

#include "secureserver.h"

class mainServerWidget : public QWidget
{
    Q_OBJECT

public:
    mainServerWidget(QWidget *parent = 0);
    ~mainServerWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartButton;

    QThread *m_WorkerThread;
    SecureServer *m_SecureServer;
};

#endif // MAINSERVERWIDGET_H
