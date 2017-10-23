#include "businessobject.h"

#include <QTimer>

BusinessObject::BusinessObject(QObject *parent)
    : QObject(parent)
    , m_RequestResponseContracts(this)
{ }
void BusinessObject::someSlot(int x)
{
    SomeSlotScopedResponder scopedResponder(m_RequestResponseContracts.someSlot());

    m_X = x;
    QTimer::singleShot(0, this, SLOT(someSlotContinuation()));

    scopedResponder.deferResponding();
}
void BusinessObject::anotherSlot()
{
    emit anotherSlotFinished(true);
}
void BusinessObject::someSlotContinuation()
{
    qDebug("someSlotContinuation called");
    SomeSlotScopedResponder scopedResponder(m_RequestResponseContracts.someSlot());
    scopedResponder.response()->setXIsEven(m_X % 2 == 0 ? true : false);
    scopedResponder.response()->setSuccess(true);
}
