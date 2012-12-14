#include "endresulttargetbusinessonthreadhelper.h"

EndResultTargetBusinessOnThreadHelper::EndResultTargetBusinessOnThreadHelper(QObject *parent) :
    QThread(parent)
{ }
void EndResultTargetBusinessOnThreadHelper::run()
{
    m_EndResultTargetBusiness = new EndResultTargetBusiness(m_SimpleArg1Saver, *m_ImplicitlySharedQStringWithPointerModifierAdded); //we advise the user to not mention parent in their constructor "declaration" i guess? we just need to make sure parent is set to zero and not to an object that calls start (accident) or the QThread ('this') that owns it... because QThread lives on a different thread than the thread it manages TODOreq

    //now that we know the constructor either saved it or used it, we decrement the reference count (the only reason we transform it into a pointer)
    //TODOreq: implicitly shared class POINTERS do not need this special stage as their passing around does not touch the reference count
    delete m_ImplicitlySharedQStringWithPointerModifierAdded;


    emit objectOnThreadEndResultTargetBusinessInstantiated();

    QThread::run();

    delete m_EndResultTargetBusiness;

    m_EndResultTargetBusiness = 0;
}
void EndResultTargetBusinessOnThreadHelper::start(int simpleArg1, QString implicitlySharedQString, QThread::Priority priority)
{
    if(!isRunning())
    {
        //generated assignings for simple args are easy
        m_SimpleArg1Saver = simpleArg1;

        //so are implicitly shared ones, once you know what to do. we "new" them now so we can "delete" them later. we need to be able to delete them without this class going out of scope, which is the sole reason we make them pointers
        m_ImplicitlySharedQStringWithPointerModifierAdded = new QString(implicitlySharedQString);

        QThread::start(priority);
    }
}
