#include "businessobject2.h"

#include <QCoreApplication>
#include <QDebug>

BusinessObject2::BusinessObject2(QObject *parent) :
    QObject(parent)
{ }
void BusinessObject2::handleSomeSlotFinished(bool success)
{
    qDebug() << "success:" << (success ? "true" : "false");
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
