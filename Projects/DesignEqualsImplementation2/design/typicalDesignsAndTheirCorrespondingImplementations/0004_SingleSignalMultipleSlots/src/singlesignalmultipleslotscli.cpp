#include "singlesignalmultipleslotscli.h"

#include "foosignalclass.h"
#include "barslotclass.h"
#include "zedslotclass.h"

SingleSignalMultipleSlotsCli::SingleSignalMultipleSlotsCli(QObject *parent)
    : QObject(parent)
{
    FooSignalClass *fooSignalClass = new FooSignalClass(this);
    BarSlotClass *barSlotClass = new BarSlotClass(this);
    ZedSlotClass *zedSlotClass = new ZedSlotClass(this);

    connect(fooSignalClass, SIGNAL(fooSignal()), barSlotClass, SLOT(barSlot0()));
    connect(fooSignalClass, SIGNAL(fooSignal()), zedSlotClass, SLOT(zedSlot()));
    connect(fooSignalClass, SIGNAL(fooSignal()), barSlotClass, SLOT(barSlot1()));

    QMetaObject::invokeMethod(fooSignalClass, "fooSlot");
}
