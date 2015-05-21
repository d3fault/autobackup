#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "qtoriginatingcrossdomainthreadsafeapicall.h"

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt : public QtOriginatingCrossDomainThreadSafeApiCall /*is responsible for context/thread and the slot name for the callback and for calling the right api on the business*/
{
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, const char *slotCallback);
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
