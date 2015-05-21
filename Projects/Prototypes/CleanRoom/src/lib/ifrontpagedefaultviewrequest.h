#ifndef IFRONTPAGEDEFAULTVIEWREQUEST_H
#define IFRONTPAGEDEFAULTVIEWREQUEST_H

#include "icleanroomrequest.h"

//#include <QVariantList>

#include "cleanroom.h"

class CleanRoom;

class IFrontPageDefaultViewRequest : public ICleanRoomRequest
{
public:
    IFrontPageDefaultViewRequest(CleanRoom *cleanRoom)
        : ICleanRoomRequest(cleanRoom)
    { }
    void invokeSlotThatHandlesRequest();
    virtual void respond(QStringList frontPageDocs)=0;
#if 0
    void convertResponseArgsFromVariantListToApiSpecificTypes/*_AndThenCallThisSlotWithThemAwwShitNvmIcantBoostWtThatWay*/(QVariantList responseArgsAsVariantList)
    {
        //the rpc generator 'knows' which list items are which response args
        QStringList frontPageDocs = responseArgsAsVariantList.first();
    }
#endif
};

#endif // IFRONTPAGEDEFAULTVIEWREQUEST_H
