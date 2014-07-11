#ifndef COMPUTER_H
#define COMPUTER_H

#include "ilogicprocessor.h"

class Computer : public ILogicProcessor
{
public:
    Computer();
protected:
    virtual void processLogic(ILogic *logic);
};

#endif // COMPUTER_H
