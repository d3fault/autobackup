#ifndef CLEANTHREADINGEXAMPLETEST_H
#define CLEANTHREADINGEXAMPLETEST_H

#include <QObject>
#include <QThread> //aldfjasdlf
#include <QCoreApplication>

#define DECLARE_INTENT_TO_USE_OBJECT_ON_THREAD(UserObjectType) \
    class UserObjectType##OnThreadHelper : public QThread \
    { \
        Q_##OBJECT \
    public: \
        explicit UserObjectType##OnThreadHelper(QObject *parent = 0) : QThread(parent) { } \
    protected: \
        virtual void run() \
        { \
            UserObjectType qobj; \
            emit object##UserObjectType##IsReadyForConnectionsOnly(&qobj); \
            exec(); \
        } \
    signals: \
        void object##UserObjectType##IsReadyForConnectionsOnly(UserObjectType *object); \
};

#include "objectonthreadhelper.h"
#include "cleanthreadingbackend1.h"
#include "cleanthreadingbackend2.h"
#include "cleanthreadingfrontendwidget.h"

DECLARE_INTENT_TO_USE_OBJECT_ON_THREAD(CleanThreadingBackend1)
DECLARE_INTENT_TO_USE_OBJECT_ON_THREAD(CleanThreadingBackend2)

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
    CleanThreadingBackend1OnThreadHelper m_BackendObject1ThreadHelper;
    CleanThreadingBackend2OnThreadHelper m_BackendObject2ThreadHelper;

    void cleanupBackendObjectsIfNeeded();
    bool m_CleanedUpBackends;
public slots:
    void handleBackendObject1isReadyForConnections(CleanThreadingBackend1* backend1);
    void handleBackendObject2isReadyForConnections(CleanThreadingBackend2* backend2);
    void handleAboutToQuit();
};

#endif // CLEANTHREADINGEXAMPLETEST_H
