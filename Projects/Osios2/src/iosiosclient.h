#ifndef IOSIOSCLIENT_H
#define IOSIOSCLIENT_H

#include "itimelinenode.h"

class QObject;

class IOsiosClient
{
public:
    virtual QObject *asQObject()=0;
protected: //signals
    virtual void actionOccurred(const ITimelineNode &action)=0;
};

#endif // IOSIOSCLIENT_H
