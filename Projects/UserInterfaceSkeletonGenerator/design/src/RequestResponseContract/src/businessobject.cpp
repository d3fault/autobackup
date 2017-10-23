#include "businessobject.h"

#include <QTimer>

BusinessObject::BusinessObject(QObject *parent)
    : QObject(parent)
    , m_SomeSlotRequestResponse(this)
{ }
void BusinessObject::someSlot()
{
    SomeSlotScopedResponder scopedResponder = m_SomeSlotRequestResponse.assumeResponsibilityToRespond();

    QTimer::singleShot(0, this, SLOT(someSlotContinuation()));

    scopedResponder.deferResponding();

#if 0 //early initial design/code/attempt
    EmitSignalOnDestruct scopedResponder("someSlotFinished");
    connect(&scopedResponder, &EmitSignalOnDestruct::signalToBeEmittedInDestructor, this, &BusinessObject::someSlotFinished);

    QTimer::singleShot(0, this, "someSlotContinuation");

    scopedResponder.deferResponding();
#endif
}
void BusinessObject::someSlotContinuation()
{
    qDebug("someSlotContinuation called");
    SomeSlotScopedResponder scopedResponder = m_SomeSlotRequestResponse.assumeResponsibilityToRespond();
    scopedResponder->setSuccess(true);
}
