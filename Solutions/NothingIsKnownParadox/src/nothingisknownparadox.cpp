#include "nothingisknownparadox.h"

#include "brain.h"

NothingIsKnownParadox::NothingIsKnownParadox(QObject *parent)
    : QObject(parent)
{
    Brain *brain = new Brain(this);
    connect(brain, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
}
