#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H

#include <QStringList>

#include <boost/function.hpp>

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromWt
{
public:
    CleanRoomFrontPageDefaultViewRequestFromWt(CleanRoomSession *session, boost::function<QStringList/*front page docs*/> wApplicationCallback);
private:
    CleanRoomSession *m_Session;
    boost::function<QStringList/*front page docs*/> m_WApplicationCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
