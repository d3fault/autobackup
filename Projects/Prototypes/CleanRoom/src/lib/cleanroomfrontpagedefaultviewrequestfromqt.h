#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt
{
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, const char *slotCallback);
private:
    CleanRoomSession *m_Session;
    const char *m_SlotCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
