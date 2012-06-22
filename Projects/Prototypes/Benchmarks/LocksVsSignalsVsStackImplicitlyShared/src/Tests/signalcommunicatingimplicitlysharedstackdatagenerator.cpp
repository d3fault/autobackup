#include "signalcommunicatingimplicitlysharedstackdatagenerator.h"

SignalCommunicatingImplicitlySharedStackDataGenerator::SignalCommunicatingImplicitlySharedStackDataGenerator(int loopCount, int sizeMultiplier, const QString &dataToBeMultiplied)
    : m_LoopCount(loopCount), m_SizeMultiplier(sizeMultiplier)
{
    m_StartingVal.append(dataToBeMultiplied);
}
void SignalCommunicatingImplicitlySharedStackDataGenerator::init()
{
    m_Interval = 0;
    m_ReplaceTo.append("b");
    m_CountChar.append("a");
}
void SignalCommunicatingImplicitlySharedStackDataGenerator::startTest()
{
    generateOne();
}
void SignalCommunicatingImplicitlySharedStackDataGenerator::generateOne()
{
    while(m_Interval < m_LoopCount) //changing this from an event based while loop to a true native while loop didn't bring as much benefit as it did in the mutex test. while it is faster than the signal based heap recycling now (this is obvious), it didn't halve it's own time like the mutex one did. in a real application we will not be in a while loop anyways... but crunching bitcoin numbers (so yes, a while loop... but we have no idea when we're going to want to send a message. it certainly isn't every time we send a message to ourselves). Actually, converting these to native while loops does nothing. I can no longer even compare the results. So sure, mutex is fastest atm... but that doesn't mean dick. Testing with actual bitcoin crunching going on would be the best test. tl;dr: inconclusive. fml. BUT IT DOES APPEAR THAT STACKS (with d ptr) ARE PRETTY FUCKING SLOW ;-p... I THINK THAT'S ALL I NEEDED TO KNOW...
    {
        QString theBytes;
        for(int i = 0; i < m_SizeMultiplier; ++i)
        {
            theBytes.append(m_StartingVal);
        }

        //change every A to a B
        //theBytes.replace("a", m_ReplaceTo);
        m_Count = theBytes.count(m_CountChar);
        emit bytesGenerated(theBytes);
        ++m_Interval;
        //QMetaObject::invokeMethod(this, "startTest", Qt::QueuedConnection);
    }
}
