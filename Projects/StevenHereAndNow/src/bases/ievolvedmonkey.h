#ifndef IEVOLVEDMONKEY_H
#define IEVOLVEDMONKEY_H

#include "ilawsofphysics.h"

class IEvolvedMonkey : /*skipping a few in between */ public ILawsOfPhysics
{
public:
    IEvolvedMonkey();
protected:
    virtual ILawsOfPhysics *implementation();
};

#endif // IEVOLVEDMONKEY_H
