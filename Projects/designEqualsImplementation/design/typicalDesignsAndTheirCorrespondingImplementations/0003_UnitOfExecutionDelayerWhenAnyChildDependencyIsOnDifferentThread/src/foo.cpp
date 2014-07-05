#include "foo.h"

#include "bar.h"

Foo::Foo(QObject *parent)
    : QObject(parent)
    , m_BarThread(new ObjectOnThreadGroup(this))
{
    m_BarThread->addObjectOnThread<Bar>("handleBarReadyForConnections");
}
void Foo::fooSlot(const QString &cunt)
{
    //The unit of execution delaying was "pushed left", but if Zop didn't exist then it'd be done here. Since it was pushed left, we know it's already done and that Bar is initialized
    QMetaObject::invokeMethod(m_Bar, "barSlot", Q_ARG(QString, cunt));
}
void Foo::handleBarReadyForConnections(QObject *barAsQObject)
{
    m_Bar = static_cast<Bar*>(barAsQObject);
    connect(m_Bar, SIGNAL(barSignal(bool)), this, SLOT(handleBarSignal(bool)));

    //After iterating a list or whatever (object on thread grouop might help, but might not (similar in any case)):
    emit allChildrenOnDifferentThreadsInitialized(); //auto-generated signal, user never sees. only generated when threading is involved in use case
}
void Foo::handleBarSignal(bool success)
{
    emit fooSignal(success);
}
