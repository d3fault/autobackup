#ifndef EASYTREETEST_H
#define EASYTREETEST_H

#include <QObject>
#include <QCoreApplication>
#include <QThread>

#include "easytree.h"
#include "mainwidget.h"

class EasyTreeTest : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeTest(QObject *parent = 0);
    void start();
    ~EasyTreeTest();
private:
    EasyTree *m_EasyTree;
    mainWidget *m_MainWidget;
    QThread *m_ThreadBusiness;
signals:
    
private slots:
    void handleAboutToQuit();
};

#endif // EASYTREETEST_H
