#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H

#include "icleanroomfrontpagedefaultviewrequest.h"

#include <QStringList>

#include <boost/function.hpp>

class ICleanRoom;

class CleanRoomFrontPageDefaultViewRequestFromWt : public ICleanRoomFrontPageDefaultViewRequest
{
public:
    CleanRoomFrontPageDefaultViewRequestFromWt(ICleanRoom *cleanRoom, CleanRoomSession parentSession, const std::string &wtSessionId, boost::function<void (QStringList/*frontPageDocs*/)> wApplicationCallback, double someArg0);
    void respond(QStringList frontPageDocs);
private:
    std::string m_WtSessionId;
    boost::function<void (QStringList/*frontPageDocs*/)> m_WApplicationCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
