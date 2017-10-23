#include "businessobject.h"

#include <QTimer>

using namespace BusinessObjectRequestResponseContract;

//TODOreq: when UserInterfaceSkeletonGenerator runs, we shouldn't generate ANY glue dirs/files if there aren't any RequestResponse contracts that necessicate them! vanilla signals/slots shouldn't have to pay the price!
//TODOmb: maybe we'll generate the skeleton BusinessObject class declaration too, since we're moving far beyond "copy/paste this template function that establishes connections"; now gotta #include "businessobjectrequestresponsecontracts.h" and declare the corresponding member.. and initialize it during constructor... and yea. I mean ofc EXPLAINING to copy/paste those into an already existing BusinessObject is useful (because code has probably already been written), but generating a BusinessObject skeleton (for when code hasn't yet been written) is becoming more and more appealing the more BusinessObject has to be modified to use this shit. and hell, even generating the BusinessObject skeleton and INSTRUCTING them to copy/paste from it into their already-coded-some BusinessObject is probably easier on the eyes/mind (than reading my stdout instructions)
BusinessObject::BusinessObject(QObject *parent)
    : QObject(parent)
    , m_RequestResponseContracts(this)
{ }
void BusinessObject::someSlot(int x)
{
    SomeSlotScopedResponder scopedResponder = m_RequestResponseContracts.someSlot().assumeResponsibilityToRespond();
    //TO DOnereq: say I have another object BusinessObject69 and it TOO has a slot named "someSlot", but with different args. having the type be called SomeSlotScopedResponder means there'd be 2 incompatible definitions of SomeSlotScopedResponder. I could solve this by prepending the BusinessObject part, eg BusinessObjectSomeSlotScopedResponder, but that seems wordy as fuck. maybe I could put them in a namespace? either one of these work, but namespaces has the advantage of potentially less typing by using the "using namespace BusinessObject;" command in source files. HOWEVER I don't like _either_ of these solutions, meh. fukken private classes (a class declared within private section of another class) man!! private classes would work maybe, but I'm trying to keep BusinessObject's decl/defin as free of auto-gen'd glue code as possible. so uhh BusinessObjectRequestResponseContracts::SomeSlotScopedResponder maybe? still wordy unless you use "auto". might as well use the BusinessObjectSomeSlotScopedResponder name at that point (since I think private classes suck). ok decided on namespaces because it ALLOWS wordiness to be evaded, and is still compatible with "auto" if you want

    m_X = x;
    QTimer::singleShot(0, this, SLOT(someSlotContinuation()));

    scopedResponder.deferResponding();
}
void BusinessObject::anotherSlot()
{
    //TODOreq: AnotherSlotScopedResponder scopedResponder = m_RequestResponseContracts.anotherSlot().assumeResponsibilityToRespond();
    //scopedResponder->setSuccess(true);

    emit anotherSlotFinished(true); //hack/lazy, doesn't belong in generated code
}
void BusinessObject::someSlotContinuation()
{
    qDebug("someSlotContinuation called");
    SomeSlotScopedResponder scopedResponder = m_RequestResponseContracts.someSlot().assumeResponsibilityToRespond();
    scopedResponder->setXIsEven(m_X % 2 == 0 ? true : false);
    scopedResponder->setSuccess(true);
}
