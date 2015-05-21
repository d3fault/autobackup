#ifndef CLEANROOMNEWSESSIONREQUESTFROMWT_H
#define CLEANROOMNEWSESSIONREQUESTFROMWT_H

#include "icleanroomnewsessionrequest.h"

#include <boost/function.hpp>

class CleanRoomSession;

class CleanRoomNewSessionRequestFromWt : public ICleanRoomNewSessionRequest
{
public:
    CleanRoomNewSessionRequestFromWt(CleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (CleanRoomSession*)> wApplicationCallback)
        : ICleanRoomNewSessionRequest(cleanRoom)
        , m_WtSessionId(wtSessionId)
        , m_WApplicationCallback(wApplicationCallback)
    { }
    void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
private:
    std::string m_WtSessionId;
    boost::function<void (CleanRoomSession*)> m_WApplicationCallback;
};

#endif // CLEANROOMNEWSESSIONREQUESTFROMWT_H
