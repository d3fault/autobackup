#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThread>

#include "testdriver.h"

class mainWidget : public QWidget
{
    Q_OBJECT
public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartButton; //init on construct, but only start the test when I say (so init'ing doesn't get counted in tests)

    QThread *m_TestDriverThread;
    TestDriver *m_TestDriver;
};

#endif // MAINWIDGET_H
