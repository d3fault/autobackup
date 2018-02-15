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
    int c = 0;
    for(auto &&i : someWidgetListValues)
    {
        qDebug() << c;
        qDebug() << "SomeWidgetList_AnEntry0:" << i.SomeWidgetList_AnEntry0;
        qDebug() << "SomeWidgetList_AnotherEntry1:" << i.SomeWidgetList_AnotherEntry1;
        ++c;
    }
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
