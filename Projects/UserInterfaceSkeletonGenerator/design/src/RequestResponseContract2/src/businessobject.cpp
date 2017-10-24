#include "businessobject.h"

#include <QTimer>

using namespace BusinessObjectRequestResponseContracts;

BusinessObject::BusinessObject(QObject *parent)
    : QObject(parent)
    , m_Contracts(this)
{ }
void BusinessObject::someSlot(int x)
{
    SomeSlotScopedResponder scopedResponder(m_Contracts.someSlot());

    m_X = x;
    QTimer::singleShot(0, this, SLOT(someSlotContinuation()));

    scopedResponder.deferResponding();
}
void BusinessObject::anotherSlot()
{
    //TODOreq: AnotherSlotScopedResponder scopedResponder(m_Contracts.anotherSlot());
    //scopedResponder.response()->setSuccess(true);

    emit anotherSlotFinished(true); //hack/lazy, doesn't belong in generated code
}
void BusinessObject::someSlotContinuation()
{
    qDebug("someSlotContinuation called");
    SomeSlotScopedResponder scopedResponder(m_Contracts.someSlot());
    scopedResponder.response()->setXIsEven(m_X % 2 == 0 ? true : false);
    scopedResponder.response()->setSuccess(true);
}
