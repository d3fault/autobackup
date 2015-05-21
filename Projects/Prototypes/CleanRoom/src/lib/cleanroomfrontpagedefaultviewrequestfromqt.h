#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "ifrontpagedefaultviewrequest.h"

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt : public IFrontPageDefaultViewRequest
{
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const QString &slotCallback);
    //void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
    void respond(QStringList frontPageDocs);
private:
    QObject *m_ObjectCallback;
    std::string m_SlotCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
