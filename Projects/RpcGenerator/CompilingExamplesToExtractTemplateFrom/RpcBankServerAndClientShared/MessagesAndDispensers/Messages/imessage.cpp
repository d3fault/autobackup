#include "imessage.h"

IMessage::IMessage(QObject *owner)
    : IRecycleableAndStreamable(owner)
{ }

//TODOreq: if i release rpc generator publically, it's vital that I properly document that NO CODE SHOULD EVER MODIFY A MESSAGE AFTER DELIVER() HAS BEEN CALLED. Qt's implicit sharing protects us from this... but we aren't using it!!!!! We're using a custom recycling technique
//^^^some solutions to this would be to protect members setters with bools and then spit out some nasty error or assert() or something.... but this is increased overhead that i don't really want to pay.
//^^^^^a solution to not wanting to pay that is to allow Rpc Generator to spit out "debug" rpc generated code and "production" rpc generated code. production code does not have those checks, simple. there are probably other similar optimizations/safety-helpers i could whip out with that debug/production shit :)
void IMessage::deliver()
{
    emit deliverSignal();
}
#if 0
void IMessage::fail(quint32 failedReasonEnum)
{
    this->FailedReasonEnum = failedReasonEnum;
}
#endif
