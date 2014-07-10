#ifndef ILAWSOFPHYSICS_H
#define ILAWSOFPHYSICS_H

class ILawsOfPhysics
{
public:
    ILawsOfPhysics();
protected:
    virtual ILawsOfPhysics* implementation()=0;
};

#endif // ILAWSOFPHYSICS_H
