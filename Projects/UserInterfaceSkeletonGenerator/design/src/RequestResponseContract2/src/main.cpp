#include <QCoreApplication>

#include <QDebug>

#include "businessobject.h"
#include "businessobject2.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BusinessObject b;
    BusinessObject2 b2;

    QObject::connect(&b, &BusinessObject::someSlotFinished, &b2, &BusinessObject2::handleSomeSlotFinished);
    QObject::connect(&b2, &BusinessObject2::anotherSlotRequested, &b, &BusinessObject::anotherSlot);
    QObject::connect(&b, &BusinessObject::anotherSlotFinished, &b2, &BusinessObject2::handleAnotherSlotFinished);

    int x = qrand();
    qDebug() << "x:" << x;

    b.someSlot(x);

    return a.exec();
}
