#ifndef IOSIOSCLIENT_H
#define IOSIOSCLIENT_H

#include <QSharedPointer>

#include "itimelinenode.h"

class QObject;

class IOsiosClient
{
public:
    virtual QObject *asQObject()=0;
protected: //signals
    virtual void actionOccurred(TimelineNode action)=0;
};

#endif // IOSIOSCLIENT_H
