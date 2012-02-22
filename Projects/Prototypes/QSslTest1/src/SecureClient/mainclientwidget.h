#ifndef MAINCLIENTWIDGET_H
#define MAINCLIENTWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QThread>
#include <QPlainTextEdit>
#include <QPushButton>

#include "secureclient.h"

class mainClientWidget : public QWidget
{
    Q_OBJECT

public:
    mainClientWidget(QWidget *parent = 0);
    ~mainClientWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartButton;

    QThread *m_WorkerThread;
    SecureClient *m_SecureClient;
};

#endif // MAINCLIENTWIDGET_H
