#include "myrandomnumbergenerator.h"
#include <QDebug>

MyRandomNumberGenerator::MyRandomNumberGenerator(QObject *parent) :
    QObject(parent), m_Timer(0)
{
}
void MyRandomNumberGenerator::start()
{
    if(!m_Timer)
    {
        m_Timer = new QTimer(this);
        connect(m_Timer, SIGNAL(timeout()), this, SLOT(generateArandomNumber()));
    }
    if(!m_Timer->isActive())
    {
        m_Timer->start(1000);
    }
}
void MyRandomNumberGenerator::generateArandomNumber()
{
    int num = qrand() % 11;
    qDebug() << QString::number(num) << " generated";
    emit randomNumberGenerated(num);
}
void MyRandomNumberGenerator::stop()
{
    if(m_Timer)
    {
        m_Timer->stop(); //this is pointless
    }
}
