#include "endresulttargettest.h"

EndResultTargetTest::EndResultTargetTest(int simpleArg1, QString implicitlySharedQString, QObject *parent) :
    QObject(parent)
{
    connect(&m_EndResultTargetBusiness, SIGNAL(objectOnThreadEndResultTargetBusinessInstantiated()), this, SLOT(handleBusinessInstantiated()));
    m_EndResultTargetBusiness.start(simpleArg1, implicitlySharedQString, QThread::InheritPriority);
}
void EndResultTargetTest::handleBusinessInstantiated()
{
    //at this point the object is instantiated and the reference count of the implicitly shared QStrings (which had to be marshalled in) is correct

    EndResultTargetBusiness *pointerToItOnAnotherThread = m_EndResultTargetBusiness.getEndResultTargetBusiness();
    Q_UNUSED(pointerToItOnAnotherThread)
}
