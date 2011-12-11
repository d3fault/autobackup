#include "inputkeyfilter.h"

bool InputKeyFilter::isAtoZOnly(Qt::Key inputKey)
{
    return ((int)inputKey >= (int)Qt::Key_A && (int)inputKey <= (int)Qt::Key_Z);
}

