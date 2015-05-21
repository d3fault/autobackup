#ifndef CLEANROOMGUISESSIONRESPONDER_H
#define CLEANROOMGUISESSIONRESPONDER_H

#include <QObject>
#include "icleanroomfrontendresponder.h"

class CleanRoomGuiResponder : public QObject, public ICleanRoomFrontEndResponder
{
    Q_OBJECT
public:
    CleanRoomGuiResponder(QObject *parentWidgetToRespondTo, const char *slotNameOnThatWidgetToHandleSessionGreeting, const char *slotNameOnThatWidgetThatHandlesSessionResponses);
    void respondSayingNewSessionSuccessful(CleanRoomSession *session);
    void respond(ICleanRoomSessionRequest *request);
signals:
    void responseRequested(ICleanRoomSessionRequest *request);
};

#endif // CLEANROOMGUISESSIONRESPONDER_H
