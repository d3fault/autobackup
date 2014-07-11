#ifndef BRAIN_H
#define BRAIN_H

#include "ilogicprocessor.h"

class Brain : public ILogicProcessor //public ILawsOfPhysics
{
public:
    Brain();
protected:
    //LogicProcessor *m_LogicProcessor;
    virtual void processLogic(ILogic *logic);
};

#endif // BRAIN_H
