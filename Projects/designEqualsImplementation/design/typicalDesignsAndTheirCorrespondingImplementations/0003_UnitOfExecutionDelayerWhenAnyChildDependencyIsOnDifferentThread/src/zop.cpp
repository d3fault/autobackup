#include "zop.h"

#include <QDebug>
#include <QCoreApplication>

#include "foo.h"

//bleh maybe it would just be much easier to move ALL thread initializing to the start of the application? since because WHAT IF SOMETHING THAT INVOKES ZOPSLOT ALSO NEEDS TO STAY IN SAME UNIT OF EXECUTION (unit of executions between use cases guaranteed? methinks NOT!)
Zop::Zop(QObject *parent)
    : QObject(parent)
    , m_Foo(new Foo(this))
    , m_AllRecursiveChildrenOnOtherThreadsAreInitialized(false)
    , m_ZopSlotCalled(false)
{
    connect(m_Foo, SIGNAL(allChildrenOnDifferentThreadsInitialized()), this, SLOT(handleAllFoosChildrenOnDifferentThreadsInitialized()));
    connect(m_Foo, SIGNAL(fooSignal(bool)), this, SLOT(handleFooSignal(bool)));
}
void Zop::zopSlotActual()
{
    QMetaObject::invokeMethod(m_Foo, "fooSlot", Q_ARG(QString, m_CuntPassThroughAutoGen));
}
//zopSlot is use case entry point, so we simply recurse "use case events" and check if any are on a different thread (easier said than done)
void Zop::zopSlot(const QString &cunt)
{
    //Intent:
    //QMetaObject::invokeMethod(m_Foo, "fooSlot", Q_ARG(QString, cunt));

    //Zop has to ask Foo if all his children [that are relevant to use case TODOreq] on different threads are done initializing, recursively. If not, wait until they are. Hard for me to keep in my head that a lot of this is auto-generated code, so some things we simply "know" from god like perspective :-P
    m_CuntPassThroughAutoGen = cunt;
    m_ZopSlotCalled = true;
    if(m_AllRecursiveChildrenOnOtherThreadsAreInitialized)
        zopSlotActual();
}
void Zop::handleAllFoosChildrenOnDifferentThreadsInitialized()
{
    //similar to how foo might have multiple children and so checks a LIST, so to do we

    //once all the children we KNOW (god knowledge) have children threads waiting to initialize... have initialized:
    m_AllRecursiveChildrenOnOtherThreadsAreInitialized = true;
    if(m_ZopSlotCalled)
        zopSlotActual();
}
void Zop::handleFooSignal(bool success)
{
    qDebug() << success;
    emit zopSignal(success);
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
