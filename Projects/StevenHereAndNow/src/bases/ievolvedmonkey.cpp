#include "ievolvedmonkey.h"

IEvolvedMonkey::IEvolvedMonkey()
    : ILawsOfPhysics()
{
    m_Organs.append(new Brain()); //wait what? am i just one giant laws of physics, or many of them? both answers make sense actually
}
ILawsOfPhysics *IEvolvedMonkey::implementation()
{
    return this;
}
