//THIS FILE IS NOT _GENERATED_, BUT IS USED ONLY FOR TESTING COMPILATION

#include "firstnamelastnameqobject.h"

#include <QDebug>
#include <QCoreApplication>

FirstNameLastNameQObject::FirstNameLastNameQObject(QObject *parent)
    : QObject(parent)
{ }
void FirstNameLastNameQObject::someSlot(const QString &firstName, const QString &lastName, const QStringList &top5Movies, const QString &favoriteDinner, const QString &favoriteLunch, const QList<SomeWidgetListEntryType> &someWidgetListValues)
{
    qDebug() << "backend says first name is:" << firstName;
    qDebug() << "backend says last name is:" << lastName;
    qDebug() << "bakcned says top5Movies is:" << top5Movies;
    qDebug() << "backend says favoriteDinner is:" << favoriteDinner;
    qDebug() << "backend says favoriteLunch is:" << favoriteLunch;
    //qDebug() << "backend says someWidgetListValues is:" << someWidgetListValues;
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
