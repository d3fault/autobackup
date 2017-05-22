#include "fingeriteratororderedbymostusedfingers.h"

//right-handed master race
bool FingerIteratorOrderedByMostUsedFingers::hasNext()
{
    if(m_InitialState)
        return true;
    if(m_Finger == Finger::LeftPinky_Finger0) //ignore the "0" in this, it really is our "least used finger" (therefore "last") in this context. but from "left to right" it is the "first"/0th, kek
    {
        return false;
    }
    return true;
}
Finger FingerIteratorOrderedByMostUsedFingers::next()
{
    if(m_InitialState)
    {
        m_InitialState = false;
        m_Finger = Finger::RightIndex_Finger6;
        return m_Finger;
    }
    m_Finger = privateNextFinger();
    return m_Finger;
}
void FingerIteratorOrderedByMostUsedFingers::reset() //I had this called "toFront" at first, but wouldn't that mean I'm pointing to RightIndexFinger? I guess not (that's not how Qt iterators work) but the wording still kinda implies yes... so reset is clearer
{
    m_InitialState = true;
}
Finger FingerIteratorOrderedByMostUsedFingers::privateNextFinger()
{
    switch(m_Finger)
    {
        case Finger::Finger6_RightIndex:
        return Finger::LeftIndex_Finger3;
        break;
        case Finger::Finger3_LeftIndex:
        return Finger::Finger7_RightMiddle;
        break;
        case Finger::Finger7_RightMiddle:
        return Finger::LeftMiddle_Finger2;
        break;
        case Finger::Finger2_LeftMiddle:
        return Finger::Finger5_RightThumb;
        break;
        case Finger::Finger5_RightThumb:
        return Finger::Finger4_LeftThumb;
        break;
        case Finger::Finger4_LeftThumb:
        return Finger::Finger8_RightRing;
        break;
        case Finger::Finger8_RightRing:
        return Finger::Finger1_LeftRing;
        break;
        case Finger::Finger1_LeftRing:
        return Finger::Finger9_RightPinky;
        break;
        case Finger::Finger9_RightPinky:
        return Finger::Finger0_LeftPinky;
        break;
        case Finger::Finger0_LeftPinky:
        return Finger::Finger6_RightIndex; //should never get here (unless the user of this class forgets to check hasNext() themselves), but it's best to wrap around to beginning in case they use this class wrong
        break;
    }
    return Finger::RightIndex_Finger6; //should (will) never get here
}
