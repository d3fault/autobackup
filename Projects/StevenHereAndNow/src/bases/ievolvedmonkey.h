#ifndef IEVOLVEDMONKEY_H
#define IEVOLVEDMONKEY_H

#include "ilawsofphysics.h"

#include <QList>

class Brain;

class IEvolvedMonkey : /*skipping a few in between */ public ILawsOfPhysics
{
public:
    IEvolvedMonkey();
protected:
    QList<Organ*> m_Organs;

    virtual ILawsOfPhysics *implementation();
};

#endif // IEVOLVEDMONKEY_H
