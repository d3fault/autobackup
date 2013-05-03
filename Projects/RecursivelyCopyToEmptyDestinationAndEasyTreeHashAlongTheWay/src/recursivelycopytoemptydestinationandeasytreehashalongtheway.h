#ifndef RECURSIVELYCOPYTOEMPTYDESTINATIONANDEASYTREEHASHALONGTHEWAY_H
#define RECURSIVELYCOPYTOEMPTYDESTINATIONANDEASYTREEHASHALONGTHEWAY_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "recursivelycopytoemptydestinationandeasytreehashalongthewaybusiness.h"
#include "mainwidget.h"

class RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(QObject *parent = 0);
private:
    ObjectOnThreadHelper<RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness> m_BusinessThread;
    MainWidget m_Gui;
private slots:
    void handleBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // RECURSIVELYCOPYTOEMPTYDESTINATIONANDEASYTREEHASHALONGTHEWAY_H
