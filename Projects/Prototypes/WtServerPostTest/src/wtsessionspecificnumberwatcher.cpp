#include "wtsessionspecificnumberwatcher.h"

WtSessionSpecificNumberWatcher::WtSessionSpecificNumberWatcher(std::string wtSessionId, WServer &server, ServerPostCallback callback)
    : m_WtSessionId(wtSessionId), m_WtServer(server), m_WtSessionCallback(callback)
{
    m_NumberWatchingListMutex = new QMutex();
}
void WtSessionSpecificNumberWatcher::watchNumber(int numberToWatch)
{
    //QMutexLocker scopedLock(m_NumberWatchingListMutex);
    m_NumberWatchingListMutex->lock();

    if(!m_NumbersBeingWatched.contains(numberToWatch) /*don't insert same number twice*/)
    {
        m_NumbersBeingWatched.append(numberToWatch);
    }
    //else, we could do a post saying "number already being watched" if we wanted to...

    m_NumberWatchingListMutex->unlock();
}
void WtSessionSpecificNumberWatcher::handleGeneratedNumber(int number)
{
    m_NumberWatchingListMutex->lock();

    if(m_NumbersBeingWatched.contains(number))
    {
        //emit numberSeen ;-)
        m_WtServer.post(m_WtSessionId, boost::bind(m_WtSessionCallback, number));
    }

    m_NumberWatchingListMutex->unlock();
}
