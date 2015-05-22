#include "%API_NAME_LOWERCASE%.h"

#include "%API_NAME_LOWERCASE%session.h"
#include "i%API_NAME_LOWERCASE%newsessionrequest.h"

%API_CALLS_HEADER_INCLUDES%

%API_NAME%::%API_NAME%(QObject *parent)
    : QObject(parent)
{ }
void %API_NAME%::initializeAndStart()
{
    //wait for business to become ready etc, then:
    emit readyForSessions();
}
void %API_NAME%::newSession(I%API_NAME%NewSessionRequest *request)
{
    request->respond(new %API_NAME%Session(this));
}
%API_CALLS_DEFINITIONS%
%API_CALLS_RESPONSE_DEFINITIONS%
