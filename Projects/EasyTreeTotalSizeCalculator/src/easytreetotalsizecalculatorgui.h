#ifndef EASYTREETOTALSIZECALCULATORGUI_H
#define EASYTREETOTALSIZECALCULATORGUI_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "easytreetotalsizecalculatorwidget.h"
#include "easytreetotalsizecalculator.h"

class EasyTreeTotalSizeCalculatorGui : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeTotalSizeCalculatorGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<EasyTreeTotalSizeCalculator> m_BusinessThread;
    EasyTreeTotalSizeCalculatorWidget m_Gui;
private slots:
    void handleEasyTreeTotalSizeCalculatorBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // EASYTREETOTALSIZECALCULATORGUI_H
