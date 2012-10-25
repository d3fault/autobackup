#ifndef CLEANTHREADINGEXAMPLETEST_H
#define CLEANTHREADINGEXAMPLETEST_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "cleanthreadingbackend1.h"
#include "cleanthreadingbackend2.h"
#include "cleanthreadingfrontendwidget.h"

//The CleanThreadingExampleTest class exists for a few reasons, but isn't strictly necessary:
//a) To keep main() as empty/clean/concise as possible. To provide a consistent entry point into your code
//b) To keep the GUI and backends oblivious of each other, we are in charge of setting up the connections between them. This helps maximize code reuse for both your GUIs and Backends
//c) Following the rapid-prototyping (also known as "component-driven") development model, it is very likely that the gui and/or backends will not be used as-is. They will most likely be incorporated (after prototyping/testing of course ;-P) into a larger application, which gives you the flexibility to decide how and where to set them up. A 'test' class gives you something you can easily prototype/test with (and refer to later on when you forget how your code works), but can throw away since it doesn't contain much actual logic
class CleanThreadingExampleTest : public QObject
{
    Q_OBJECT
public:
    explicit CleanThreadingExampleTest(QObject *parent = 0);
    ~CleanThreadingExampleTest();
private:
    //GUI QWidget. He is completely oblivious of the back end objects and what thread they live on
    CleanThreadingFrontendWidget m_Gui;

    //Our backend object TYPES are inserted directly into an ObjectOnThreadHelper template class. The helper instantiates them on the backend thread, tells us when they are ready for connections, and destroys them when the thread is ending. They are completely oblivious of the GUI and what thread he lives on. Note that these two helper objects do not live on the thread in which they create/represent. They live on the GUI thread
    ObjectOnThreadHelper<CleanThreadingBackend1> m_BackendObject1ThreadHelper;
    ObjectOnThreadHelper<CleanThreadingBackend2> m_BackendObject2ThreadHelper;

    void cleanupBackendObjectsIfNeeded();
    bool m_CleanedUpBackends;
public slots:
    void handleBackendObject1isReadyForConnections();
    void handleBackendObject2isReadyForConnections();
    void handleAboutToQuit();
};

#endif // CLEANTHREADINGEXAMPLETEST_H
