#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "ifrontpagedefaultviewrequest.h"

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt : public IFrontPageDefaultViewRequest
{
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, QObject *objectCallback, const QString &slotCallback);
    void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
private:
    QObject *m_ObjectCallback;
    std::string m_SlotCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
