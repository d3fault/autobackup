#include "signalcommunicatingheaprecyclingdataprocessor.h"

SignalCommunicatingHeapRecyclingDataProcessor::SignalCommunicatingHeapRecyclingDataProcessor(QObject *parent) :
    QObject(parent)
{
}
void SignalCommunicatingHeapRecyclingDataProcessor::init()
{
    m_ReplaceBackTo.append("a");
}
void SignalCommunicatingHeapRecyclingDataProcessor::processGeneratedData(QString *generatedData)
{
    //change all B's to A's
    generatedData->replace("b", m_ReplaceBackTo);
    emit doneWithData(generatedData);
}
