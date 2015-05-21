#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "icleanroomfrontpagedefaultviewrequest.h"

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt : public ICleanRoomFrontPageDefaultViewRequest
{
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, QObject *objectCallback, const char *slotCallback);
    void respondActual(QVariantList responseArgs);
private:
    QObject *m_ObjectCallback;
    const char *m_SlotCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
