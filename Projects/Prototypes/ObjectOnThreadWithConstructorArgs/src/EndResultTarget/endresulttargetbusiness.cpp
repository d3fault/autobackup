#include "endresulttargetbusiness.h"

EndResultTargetBusiness::EndResultTargetBusiness(int simpleArg1, QString implicitlySharedQString, QObject *parent) :
    QObject(parent)
{
    m_SimpleArg1ToBeUsedByThisClass = simpleArg1;
    m_ImplicitlySharedQStringToBeUsedByThisClass = implicitlySharedQString;
}
