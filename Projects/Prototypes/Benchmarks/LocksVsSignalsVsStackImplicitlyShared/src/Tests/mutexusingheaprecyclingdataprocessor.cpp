#include "mutexusingheaprecyclingdataprocessor.h"

MutexUsingHeapRecyclingDataProcessor::MutexUsingHeapRecyclingDataProcessor(MutexUsingHeapRecyclingDataGenerator *listOwner)
    : m_ListOwner(listOwner)
{
}
void MutexUsingHeapRecyclingDataProcessor::init()
{
    m_ReplaceBackTo.append("a");
}
void MutexUsingHeapRecyclingDataProcessor::processGeneratedData(QString *generatedData)
{
    //generatedData->replace("b", m_ReplaceBackTo);
    m_Count = generatedData->count(m_ReplaceBackTo);
    //emit doneWithData(generatedData);
    m_ListOwner->recycledUsed(generatedData);
}
