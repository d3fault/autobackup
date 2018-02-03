#include "firstnamelastnameqobject.h"

#include <QDebug>
#include <QCoreApplication>

FirstNameLastNameQObject::FirstNameLastNameQObject(QObject *parent)
    : QObject(parent)
{ }
void FirstNameLastNameQObject::someSlot(const QString &firstName, const QString &lastName)
{
    qDebug() << "backend says first name is:" << firstName;
    qDebug() << "backend says last name is:" << lastName;
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
