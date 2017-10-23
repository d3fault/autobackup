#include "businessobject2.h"

#include <QCoreApplication>
#include <QDebug>

BusinessObject2::BusinessObject2(QObject *parent) :
    QObject(parent)
{ }
void BusinessObject2::handleSomeSlotFinished(bool success, bool xIsEven)
{
    qDebug() << "x is even:" << (xIsEven ? "true" : "false");
    qDebug() << "success:" << (success ? "true" : "false");
    emit anotherSlotRequested();
}
void BusinessObject2::handleAnotherSlotFinished(bool success)
{
    qDebug() << "another slot finished: " << (success ? "successfully" : "unsuccessfully");
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
