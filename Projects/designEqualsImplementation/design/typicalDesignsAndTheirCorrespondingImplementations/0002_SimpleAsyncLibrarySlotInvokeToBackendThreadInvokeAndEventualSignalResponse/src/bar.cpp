#include "bar.h"

Bar::Bar(QObject *parent)
    : QObject(parent)
{ }
void Bar::barSlot(const QString &cunt)
{
    if(cunt == "yolo")
        emit barSignal(true);
    else
        emit barSignal(false);
}
