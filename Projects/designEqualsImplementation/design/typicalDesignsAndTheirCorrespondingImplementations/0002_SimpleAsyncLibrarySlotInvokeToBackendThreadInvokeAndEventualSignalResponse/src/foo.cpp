#include "foo.h"

#include "bar.h"

Foo::Foo(QObject *parent)
    : QObject(parent)
    , m_BarThread(new ObjectOnThreadGroup(this))
    , m_FooSlotInvoked(false)
    , m_BarIsReady(false)
{
    m_BarThread->addObjectOnThread<Bar>("handleBarReadyForConnections");
    m_BarThread->doneAddingObjectsOnThreads();
}
//transparently auto-generated, including control bools to get here exactly once zomgmindfuck-butpossiblewoot
void Foo::doTheRestOfFooSlot()
{
    //chances are pretty good this is not TECHNICALLY the same unit of execution (in any case, it isn't guaranteed) as "fooSlot", BUT fooSlot 'became' basically just a parameter grabber [and control checker/router], which in turn made this method our ACTUAL fooSlot. Note that on the dia design that fooSlot, diagnosticSignalX, and barSlot are all on the same unit of execution. TODOreq: whenever [something!] -- but basically all I can pull out of that is that "all of the ACTUAL statements have to come after m_Bar (and there could be N equivalent which means a ton of auto-generated bools (I'll just code for one for now, KISS)) has told us he's ready on his thread", even if they come before Bar's invocation design-wise!!! That might not be entirely warranted and could be in fact stupid and unoptimal, but it is certainly the safest route to evade sync errors and the like...
    //TODOreq: what if fooSlot emitted some signal back to a caller (left on the design) that needed to retain the same unit of execution? would that mean that Bar has to be initialized before said object (let's call it Zop) can even begin it's unit of execution?
    //TODOreq: SINCE bar is on a different thread, perhaps we only need to keep a queue of "bar slot invokes" (queue because it could have happened multiple times before bar was ready!) and we do just emit diagnosticSignalX right before enqueuing the barSlot invoke each time (so in this example, the first line in fooSlot would be emit diagnosticSignalX). Perhaps since barSlot invoke IS a different unit of execution, we don't need to delay our own unit of execution's statements to wait for bar to be ready?
    //TODOreq: gets extremely complicated to think that Bar might want to listen to diagnosticSignalX in a different use case!!! if that were the case, we definitely would need to wait for Bar to finish initailzing before emitting diagnosticSignalX (just like I'm doing now (and this is why I said waiting is safer)). SO going back to what I was saying a little bit ago, WHATEVER use case entry point starts a single unit of execution (Zop), even if indirectly related to Bar (Zop hasA or is-neighbors-with Foo, Foo hasA Bar [on different thread]), needs to wait for Bar to finish initializing before beginning! Man that's going to be a bitch to hack (but possible, yes)
    //my head hurts
    //I _COULD_ check if other use cases involving Bar utilize diagnosticSignalX and then act accordingly, BUT that's impossible/extremely-difficult if the user modifies the generated C++ directly (or "drops down to C++ mode")
    //Essentially-but-kinda-OT (extremely common sense multi threading statement inc): any time a cross thread call is made, you are not guaranteed to be in the same unit of execution (can get lucky though) if/when that call returns
    emit diagnosticSignalX();
    QMetaObject::invokeMethod(m_Bar, "barSlot", Q_ARG(QString, m_CuntPassThroughAutoGen));
}
void Foo::fooSlot(const QString &cunt)
{
    //just:
    m_CuntPassThroughAutoGen = cunt;

    //OR:
    m_FooSlotInvoked = true;
    if(m_BarIsReady)
    {
        doTheRestOfFooSlot();
    }
}
void Foo::handleBarReadyForConnections(QObject *barAsQObject)
{
    //Since I'm generating code, I can finally solve the "initialize thread WITH object constructor args" problem, but that still doesn't make Foo compliant with RAII. I hope to at least make Foo appear to use RAII from the design perspective, hiding all the thread initialization stuff from the user

    //myActualConstructor(); ?? should diagnosticSignalX be emitted in Foo's real constructor, or this fake/delayed-until-bar-is-ready constructor? first instinct is to say constructor, BUT then that would mean that emit is on a different unit of execution!!! Might not be a big deal now (obviously isn't), but in real world scenarios it could be a huge deal (I really don't know, haven't thought about it hard enough).
    //In any case, this is definitely when we'd do:
    m_Bar = static_cast<Bar*>(barAsQObject);
    connect(m_Bar, SIGNAL(barSignal(bool)), this, SLOT(handleBarSignal(bool)));

    //QMetaObject::invokeMethod(m_Bar, "barSlot", Q_ARG(QString, m_CuntPassThroughAutoGen));

    //instead of invokeMethod (meeting up with Foo's "OR"):
    m_BarIsReady = true;
    if(m_FooSlotInvoked)
    {
        doTheRestOfFooSlot();
    }
}
void Foo::handleBarSignal(bool success)
{
    //Foo's second unit of execution entry-point, since barSlot definitely ran on Bar's thread
    emit fooSignal(success);
}
