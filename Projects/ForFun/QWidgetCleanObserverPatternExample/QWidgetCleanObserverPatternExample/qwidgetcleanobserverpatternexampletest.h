#ifndef QWIDGETCLEANOBSERVERPATTERNEXAMPLETEST_H
#define QWIDGETCLEANOBSERVERPATTERNEXAMPLETEST_H

#include <QCoreApplication>
#include <QObject>
#include <QThread>

#include "maincleanobserverpatternexamplewidget.h"
#include "cleanobserverpatternexamplebackenda.h"
#include "cleanobserverpatternexamplebackendb.h"

//The QWidgetCleanObserverPatternExampleTest class exists for a few reasons, but isn't strictly necessary:
//a) To keep main() as empty/clean/concise as possible. To provide a consistent entry point into your code
//b) To keep the GUI and backends oblivious of each other. We set up the connections between them
//c) Following the rapid-prototyping development model, it is very likely that the gui and/or back-ends will not be used as-is. They will most likely be incorporated (after prototyping ;-P) into a larger application, which gives you the flexibility to decide how and where to set them up. A 'test' class gives you something you can easily test/prototype with (and refer to later when you forget what's going on), but can still very easily throw away since it doesn't contain much actual logic
class QWidgetCleanObserverPatternExampleTest : public QObject
{
    Q_OBJECT
public:
    explicit QWidgetCleanObserverPatternExampleTest(QObject *parent = 0);
    void startExample();
private:
    //GUI QWidget. He is completely oblivious of the back end objects and what thread they live on
    MainCleanObserverPatternExampleWidget m_Gui;

    //Our backend objects (which will be moved to their own threads). They are completely oblivious of the GUI
    CleanObserverPatternExampleBackendA m_CleanObserverPatternExampleBackendA;
    CleanObserverPatternExampleBackendB m_CleanObserverPatternExampleBackendB;

    //Qt's thread helper class, QThread, greatly simplifies managing native threads in a cross platform manner. Note that QThread is not actually a thread, nor does it live on the thread it manages. In this case, the QThread objects live on the GUI thread
    QThread m_BackendThreadA;
    QThread m_BackendThreadB;
public slots:
    void handleAboutToQuit();
};

#endif // QWIDGETCLEANOBSERVERPATTERNEXAMPLETEST_H
