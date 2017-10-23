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
}
void BusinessObject::someSlotContinuation()
{
    qDebug("someSlotContinuation called");
    SomeSlotScopedResponder scopedResponder = m_SomeSlotRequestResponse.assumeResponsibilityToRespond();
    scopedResponder->setSuccess(true);
}
