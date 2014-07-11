#ifndef ILOGICPROCESSOR_H
#define ILOGICPROCESSOR_H

#include "../bases/ilawsofphysics.h"

#include "../bases/ilogic.h"

class ILogicProcessor
{
public:
    ILogicProcessor();
    virtual void processLogic(ILogic *logic)=0;
};

#endif // ILOGICPROCESSOR_H
