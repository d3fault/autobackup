#include "inputkeyfilter.h"

bool InputKeyFilter::isAtoZOnly(Qt::Key inputKey)
{
    return (inputKey >= Qt::Key_A && inputKey <= Qt::Key_Z);
}

