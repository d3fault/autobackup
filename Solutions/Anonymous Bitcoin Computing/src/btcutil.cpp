#include "btcutil.h"

BtcUtil::BtcUtil()
{
}
BtcUtil::doubleToQString(double input)
{
    return QString::number(input,'f',8);
}
