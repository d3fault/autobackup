#ifndef CLEANROOMGUISESSIONRESPONDER_H
#define CLEANROOMGUISESSIONRESPONDER_H

#include "icleanroomsessionresponder.h"

class CleanRoomGuiSessionResponder : public ICleanRoomSessionResponder
{
public:
    CleanRoomGuiSessionResponder(QObject *widgetAkaQObjectToRespondTo, const char *slotNameOnThatWidgetThatHandlesSessionResponses);
    void respond(ICleanRoomSessionRequest *request);
};

#endif // CLEANROOMGUISESSIONRESPONDER_H
