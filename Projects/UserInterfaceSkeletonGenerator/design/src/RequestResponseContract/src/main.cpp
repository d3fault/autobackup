#include <QCoreApplication>

#include "businessobject.h"
#include "businessobject2.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BusinessObject b;
    BusinessObject2 b2;
    QObject::connect(&b, &BusinessObject::someSlotFinished, &b2, &BusinessObject2::handleSomeSlotFinished);

    b.someSlot();
    return a.exec();
}
