#include "workinginputtestbusiness.h"

WorkingInputTestBusiness::WorkingInputTestBusiness(int simpleIntArg1, QString implicitlySharedQStringArg2, QObject *parent)
    : QObject(parent)
{
    m_SimpleIntArg1 = simpleIntArg1;
    m_ImplicitlySharedQStringArg2 = implicitlySharedQStringArg2;
}
