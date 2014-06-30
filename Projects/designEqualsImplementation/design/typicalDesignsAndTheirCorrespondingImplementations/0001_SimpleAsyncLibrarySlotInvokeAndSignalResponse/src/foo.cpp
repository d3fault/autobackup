#include "foo.h"

#include "bar.h"

Foo::Foo(QObject *parent)
    : QObject(parent)
{
    Bar *bar = new Bar(this);
    connect(this, SIGNAL(barSlotRequested(QString)), bar, SLOT(barSlot(QString)));
    connect(bar, SIGNAL(barSignal(bool)), this, SIGNAL(fooSignal(bool)));
}
void Foo::fooSlot(const QString &cunt)
{
    //TODOreq: we could JIT instantiate Bar here (and other invocations) via 'bar != 0' checking, or do it in constructor. Both methods have tradeoffs and I would think actually it depends on the specific use case. To KISS, I'm going to instantiate Bar in the constructor for now


    emit barSlotRequested(cunt); //TODOreq: auto-inserted line emitting auto-generated signal, but what if the user wants to add a custom C++ snippet to fooSlot? should the line be there (so they decide whether their c++ goes before or after the signal emitting), or should they not even see it when dropping down to C++ mode?
}
