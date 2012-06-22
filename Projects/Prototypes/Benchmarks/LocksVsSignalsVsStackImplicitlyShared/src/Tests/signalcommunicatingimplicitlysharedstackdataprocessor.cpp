#include "signalcommunicatingimplicitlysharedstackdataprocessor.h"

SignalCommunicatingImplicitlySharedStackDataProcessor::SignalCommunicatingImplicitlySharedStackDataProcessor(int loopCount)
    : m_LoopCount(loopCount)
{
}
void SignalCommunicatingImplicitlySharedStackDataProcessor::init()
{
    m_Interval = 0;
    m_ReplaceBackTo.append("a");
}
void SignalCommunicatingImplicitlySharedStackDataProcessor::processGeneratedData(QString generatedData)
{
    //change all B's to A's
    generatedData.replace("b", m_ReplaceBackTo); //fuck, we'd detach() at this point. test fails/sucks. or maybe idgaf? or i could just NOP right here? but i need to "spin cycles". also, the compiler might notice that we don't do anything and NOP this anyways...
    //emit doneWithData(generatedData);
    //we just let it go out of scope and forget about it. dgaf


    //the best test that i could do for each of these 3 tests is to write it to a socket. have some other client (server?) that just prints the results only. only so it doesn't NOP. but maybe i can think of something else instead?

    ++m_Interval;
    if(m_Interval >= m_LoopCount)
    {
        emit testFinished();
    }
}
