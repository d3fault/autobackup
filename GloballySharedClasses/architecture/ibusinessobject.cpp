#include "ibusinessobject.h"

//TODOreq: see if diamond inheritence can be solved by making the signals "protected pure virtuals", and the slots "public pure virtuals" (would require some annoying forced "fixing" in inherited classes (putting them in signals/slots sections), but would at least solve diamond inheritence problem)
IBusinessObject::IBusinessObject(QObject *parent)
    : QObject(parent)
{
    //no signals should be emitted from here for obvious reasons
}
