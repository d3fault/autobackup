#ifndef IINHERITEDTRAIT_H
#define IINHERITEDTRAIT_H

#include "../bases/ilogic.h"

#include <QList>

class IInheritedTrait : public ILogic
{
public:
    IInheritedTrait();
};

typedef QList<IInheritedTrait*> IInheritedTraits;

#endif // IINHERITEDTRAIT_H
