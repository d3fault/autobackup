#include "simpleasynclibraryslotinvokeandsignalresponsecli.h"

#include <QCoreApplication>

#include "../../foo.h"

//TODOreq: UI is not being factored into designEquals at this point, so this class is not even trying to be elegant/etc
SimpleAsyncLibrarySlotInvokeAndSignalResponseCli::SimpleAsyncLibrarySlotInvokeAndSignalResponseCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    Foo *foo = new Foo(this);
    connect(foo, SIGNAL(fooSignal(bool)), this, SLOT(handleFooSignal(bool)));
    QMetaObject::invokeMethod(foo, "fooSlot", Q_ARG(QString, QString("test"))); //or could be a permanent connection with corresponding this.fooSlotInvocationRequested signal to start everything off
}
void SimpleAsyncLibrarySlotInvokeAndSignalResponseCli::handleFooSignal(bool success)
{
    m_StdOut << QString("fooSignal indicated success = " + success ? "true" : "false") << endl;
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
