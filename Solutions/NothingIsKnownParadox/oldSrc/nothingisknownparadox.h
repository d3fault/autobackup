#ifndef NOTHINGISKNOWNPARADOX_H
#define NOTHINGISKNOWNPARADOX_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "brain.h"

class NothingIsKnownParadox : public QObject
{
    Q_OBJECT
public:
    explicit NothingIsKnownParadox(QObject *parent = 0);
private:
    ObjectOnThreadHelper<Brain> m_BrainThread;
    void userInput();
signals:
    void wantToKnowWhetherOrNotNothingIsKnown();
public slots:
    void handleBrainReadyForConnections();
    void handleAboutToQuit();
private slots:
    void handleBrainReportingWhetherOrNotNothingIsKnown(bool nothingIsKnown);
};

#endif // NOTHINGISKNOWNPARADOX_H
