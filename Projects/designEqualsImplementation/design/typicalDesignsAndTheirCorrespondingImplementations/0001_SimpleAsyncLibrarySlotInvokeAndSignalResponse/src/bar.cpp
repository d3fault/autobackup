#include "bar.h"

Bar::Bar(QObject *parent)
    : QObject(parent)
{ }
void Bar::barSlot(const QString &cunt)
{
    if(cunt == "test")
    {
        emit barSignal(true); //TODOreq: see note about mixing C++ code with these in-design signal emissions in fooSlot
    }
    else
    {
        emit barSignal(false);
    }
}
