#include "bank.h"

Bank::Bank(QObject *parent) :
    QObject(parent)
{
}
void Bank::init()
{
    //todo: init sql db i guess

    emit initialized();
}
