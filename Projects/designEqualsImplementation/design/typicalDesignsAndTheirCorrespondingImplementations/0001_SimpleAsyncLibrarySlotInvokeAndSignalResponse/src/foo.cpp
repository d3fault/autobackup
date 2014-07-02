#include "foo.h"

#include "bar.h"

Foo::Foo(QObject *parent)
    : QObject(parent)
    , m_Bar(new Bar(this))
{
    //connect(this, SIGNAL(barSlotRequested(QString)), m_Bar, SLOT(barSlot(QString)));
    //TODOoptimization: connect(bar, SIGNAL(barSignal(bool)), this, SIGNAL(fooSignal(bool))); -- this should be an opt-in (or ask on first occurance) application setting, but then again maybe not because who says that a signal is emitted next (if a slot is invoked next, we could instead connect it directly to that as the optimization instead!). if there are 20 signals chained together, the code could maybe skip 18 of the middle ones. but SHOULD IT? those 18 signals might be useful later (at this point it depends if coder is modifying the C++ by hand or is using DesignEquals (because if using DesignEquals, hooking into any of those 18 middle signals would make the optimization not doable [to 18 anymore]
    connect(m_Bar, SIGNAL(barSignal(bool)), this, SLOT(handleBarSignal(bool)));
}
void Foo::fooSlot(const QString &cunt)
{
    emit diagnosticSignalX();

    //TODOreq: we could JIT instantiate Bar here (and other invocations) via 'bar != 0' checking, or do it in constructor. Both methods have tradeoffs and I would think actually it depends on the specific use case. To KISS, I'm going to instantiate Bar in the constructor for now

    QMetaObject::invokeMethod(m_Bar, "barSlot", Q_ARG(QString, cunt));
    //emit barSlotRequested(cunt); //TODOreq: auto-inserted line emitting auto-generated signal, but what if the user wants to add a custom C++ snippet to fooSlot? should the line be there (so they decide whether their c++ goes before or after the signal emitting), or should they not even see it when dropping down to C++ mode?
    //^^^I think if they want to add C++ code before or after a slot invocation, they should add a PrivateMethod before or after it in the use case event list. That being said, our generator can/does combine lots of "slot invocations" into a single slotBody (such as this context right here). ex: we could have barSlotRequested(cunt), weabooSlotRequested(cunt), poopRequested(cunt), etc... all within this context (itself also a slot)... since this is auto-generated code
}
void Foo::handleBarSignal(bool success)
{
    emit fooSignal(success);
}
