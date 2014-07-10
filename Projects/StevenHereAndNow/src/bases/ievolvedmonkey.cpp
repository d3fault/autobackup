#include "ievolvedmonkey.h"

IEvolvedMonkey::IEvolvedMonkey()
    : ILawsOfPhysics()
{ }
ILawsOfPhysics *IEvolvedMonkey::implementation()
{
    return this;
}
