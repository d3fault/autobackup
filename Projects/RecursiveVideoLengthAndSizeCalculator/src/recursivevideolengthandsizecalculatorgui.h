#ifndef RECURSIVEVIDEOLENGTHANDSIZECALCULATORGUI_H
#define RECURSIVEVIDEOLENGTHANDSIZECALCULATORGUI_H

#include <QObject>
#include <QCoreApplication>

#include "recursivevideolengthandsizecalculator.h"
#include "recursivevideolengthandsizecalculatorwidget.h"
#include "objectonthreadhelper.h"

class RecursiveVideoLengthAndSizeCalculatorGui : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveVideoLengthAndSizeCalculatorGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<RecursiveVideoLengthAndSizeCalculator> m_BusinessThread;
    RecursiveVideoLengthAndSizeCalculatorWidget m_Gui;
signals:

public slots:
    void handleRecursiveVideoLengthAndSizeCalculatorReadyForConnections();
    void handleAboutToQuit();
};

#endif // RECURSIVEVIDEOLENGTHANDSIZECALCULATORGUI_H
