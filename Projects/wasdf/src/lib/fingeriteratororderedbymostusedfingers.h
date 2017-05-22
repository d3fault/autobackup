#ifndef FINGERITERATORORDEREDBYMOSTUSEDFINGERS_H
#define FINGERITERATORORDEREDBYMOSTUSEDFINGERS_H

#include "wasdf.h"

class FingerIteratorOrderedByMostUsedFingers
{
public:
    bool hasNext();
    Finger next();
    void reset();
private:
    Finger privateNextFinger();

    bool m_InitialState = true;
    Finger m_Finger; // = Finger::Finger6_RightIndex;
};

#endif // FINGERITERATORORDEREDBYMOSTUSEDFINGERS_H
