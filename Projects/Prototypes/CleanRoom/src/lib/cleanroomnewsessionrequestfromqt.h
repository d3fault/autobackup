#ifndef CLEANROOMNEWSESSIONREQUESTFROMQT_H
#define CLEANROOMNEWSESSIONREQUESTFROMQT_H

#include <QObject>
#include "icleanroomnewsessionrequest.h"

class CleanRoomNewSessionRequestFromQt : public ICleanRoomNewSessionRequest
{
    Q_OBJECT
public:
    CleanRoomNewSessionRequestFromQt(CleanRoom *cleanRoom, QObject *objectToRespondTo, const char *slotCallback, QObject *parent = 0)
        : ICleanRoomNewSessionRequest(cleanRoom, parent)
        , m_ObjectToRespondTo(objectToRespondTo)
        //, m_SlotCallback(slotCallback)
    {
        QObject::connect(this, SIGNAL(newSessionResponseRequested(CleanRoomSession*)), objectToRespondTo, slotCallback);
    }
    //void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
    void respond(CleanRoomSession *session);
private:
    QObject *m_ObjectToRespondTo;
    //std::string m_SlotCallback;
signals:
    void newSessionResponseRequested(CleanRoomSession *session);
};

#endif // CLEANROOMNEWSESSIONREQUESTFROMQT_H
