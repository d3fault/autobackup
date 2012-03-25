#include "randomnumbersingleton.h"
RandomNumberSingleton::RandomNumberSingleton()
{
}
RandomNumberSingleton *RandomNumberSingleton::m_pInstance = NULL;
RandomNumberSingleton * RandomNumberSingleton::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new RandomNumberSingleton();
        m_MyThread = new QThread();
        m_pInstance->moveToThread(m_MyThread);
        m_MyThread->start();
        QMetaObject::invokeMethod(m_pInstance, "init", Qt::QueuedConnection);
    }
    return m_pInstance;
}
void RandomNumberSingleton::init()
{
    m_GenerateNumberTimer = new QTimer(this);
    connect(m_GenerateNumberTimer, SIGNAL(timeout()), this, SLOT(generateNumber()));
}
void RandomNumberSingleton::generateNumber()
{
    int num = qrand() % 11;
    emit numberGenerated(num);
}
