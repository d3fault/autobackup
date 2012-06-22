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
}
void SignalCommunicatingImplicitlySharedStackDataGenerator::startTest()
{
    generateOne();
}
void SignalCommunicatingImplicitlySharedStackDataGenerator::generateOne()
{
    if(m_Interval < m_LoopCount)
    {
        QString theBytes;
        for(int i = 0; i < m_SizeMultiplier; ++i)
        {
            theBytes.append(m_StartingVal);
        }

        //change every A to a B
        theBytes.replace("a", m_ReplaceTo);
        emit bytesGenerated(theBytes);
        ++m_Interval;
        QMetaObject::invokeMethod(this, "startTest", Qt::QueuedConnection);
    }
}
