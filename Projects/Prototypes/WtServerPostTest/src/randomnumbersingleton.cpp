#include "randomnumbersingleton.h"
RandomNumberSingleton::RandomNumberSingleton()
{
}
RandomNumberSingleton *RandomNumberSingleton::m_pInstance = NULL;
QThread *RandomNumberSingleton::m_MyThread = NULL;
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
    m_GenerateNumberTimer->start(3000);
}
void RandomNumberSingleton::generateNumber()
{
    int num = qrand() % 11;
    qDebug() << "Number Generated: " << QString::number(num);
    Q_EMIT numberGenerated(num);
}
