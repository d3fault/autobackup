#include "btcutil.h"

QString BtcUtil::doubleToQString(double input)
{
    return QString::number(input,'f',8);
}
