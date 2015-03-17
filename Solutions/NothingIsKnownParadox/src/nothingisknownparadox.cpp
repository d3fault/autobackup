#include "nothingisknownparadox.h"

#include "brain.h"

NothingIsKnownParadox::NothingIsKnownParadox(QObject *parent)
    : QObject(parent)
{
    Brain *brain = new Brain(this);
    connect(brain, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    //QString theClaim(THE_CLAIM);
    //QMetaObject::invokeMethod(brain, "makeClaim", Qt::QueuedConnection, Q_ARG(QString, theClaim));
    //QMetaObject::invokeMethod(brain, "observeEnvironment", Qt::QueuedConnection);
}
