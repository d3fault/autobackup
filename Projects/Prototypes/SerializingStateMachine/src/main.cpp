#include <QCoreApplication>

#include "serializingstatemachine.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SerializingStateMachine ssm;
    Q_UNUSED(ssm)

    return a.exec();
}
