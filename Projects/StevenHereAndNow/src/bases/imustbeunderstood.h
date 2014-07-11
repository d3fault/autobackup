#ifndef IMUSTBEUNDERSTOOD_H
#define IMUSTBEUNDERSTOOD_H

#include "ilawsofphysics.h"

//TODOreq: not a "base", is inherited trait
class ICanBeUnderstood : public ILawsOfPhysics
{
    ICanBeUnderstood();
    virtual bool amUnderstood();
};

#endif // IMUSTBEUNDERSTOOD_H
