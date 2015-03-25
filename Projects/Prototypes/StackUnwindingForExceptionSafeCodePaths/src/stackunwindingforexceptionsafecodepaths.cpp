#include "stackunwindingforexceptionsafecodepaths.h"

//scoped/shared pointers can be used to do exception safe memory management; this prototype attempts to take that concept further with exception safe logic/code traversal/paths

SomeTask::SomeTask(StackUnwindingForExceptionSafeCodePaths *someTaskProcessor /* eh the task shouldn't be bound to a specific processor */)
    : m_SomeTaskProcessor(someTaskProcessor)
    , m_Success(false)
{ }
SomeTask::~SomeTask()
{
    m_SomeTaskProcessor->emitFinished(m_Success);
}
StackUnwindingForExceptionSafeCodePaths::StackUnwindingForExceptionSafeCodePaths(QObject *parent)
    : QObject(parent)
{ }
void StackUnwindingForExceptionSafeCodePaths::emitFinished(bool success)
{
    emit finished(success);
}
void StackUnwindingForExceptionSafeCodePaths::doSomeTask(int x /*or perhaps we pass in a SomeTask? but keeping them hidden/behind-the-scenes does make it ok that a task is bound to a specific processor. plus, cleaner api is cleaner: the user shouldn't care about our stack unwinding exception (had:thread) safety logic traversal*/)
{
    Q_UNUSED(x) //normally we'd do something with it ofc
    StackUndwinder stackUndwinder(new SomeTask(this));

    QMetaObject::invokeMethod(this, "bkaskfj", Qt::QueuedConnection, Q_ARG(StackUndwinder, stackUndwinder));
}
void StackUnwindingForExceptionSafeCodePaths::bkaskfj(SomeTask theTask)
{
    QList<int> someList;
    someList.append(0);
    int x = someList.at(1); //woops. throws (i think, but could be wrong lol. in any case, for demonstration purposes let's say it does). but our signal is still emitted because theTask goes out of scope and emits it


    //or even just logical deductions, nor errors
    if(!someList.isEmpty())
        return; //finished(false) implicitly emitted

    //extrapolating on the above (i like the single-lined-ness of it)
    if(!someList.isEmpty())
        return asyncError("list isn't empty"); //eh, such one lined-ness is desired, but would require this slot to have a return type, which is too large of a design change

    //it could be:
    if(!someList.isEmpty())
    {
        theTask.setErrorDescription("list isn't empty"); //this line, if "forgotten" (because 'return' is a much more important/critical line), defaults to "unknown error" or "generic error". maybe appendError so that multiple errors can occur. but honestly at this point (two lines), we might as well just emit e ourselves. --elegance;
        return;
    }


    //successful slot execution, either pass onto another slot like this one, or when done with the entire [async] task:
    theTask.setSuccess();
}
