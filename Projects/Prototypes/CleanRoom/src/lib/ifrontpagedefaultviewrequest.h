#ifndef IFRONTPAGEDEFAULTVIEWREQUEST_H
#define IFRONTPAGEDEFAULTVIEWREQUEST_H

#include "icleanroomrequest.h"

//#include <QVariantList>

#include "cleanroom.h"
#include "cleanroomsession.h"

class IFrontPageDefaultViewRequest : public ICleanRoomRequest
{
public:
    IFrontPageDefaultViewRequest(CleanRoomSession *session)
        : ICleanRoomRequest(session)
    { }
    void invokeSlotThatHandlesRequest();
#if 0
    void convertResponseArgsFromVariantListToApiSpecificTypes/*_AndThenCallThisSlotWithThemAwwShitNvmIcantBoostWtThatWay*/(QVariantList responseArgsAsVariantList)
    {
        //the rpc generator 'knows' which list items are which response args
        QStringList frontPageDocs = responseArgsAsVariantList.first();
    }
#endif
};

#endif // IFRONTPAGEDEFAULTVIEWREQUEST_H
