#include "%API_NAME_LOWERCASE%newsessionrequestfromqt.h"

void %API_NAME%NewSessionRequestFromQt::respond(%API_NAME%Session *session)
{
    emit newSessionResponseRequested(session);
}
