#include "i%API_NAME_LOWERCASE%.h"

#include "%API_NAME_LOWERCASE%session.h"
#include "i%API_NAME_LOWERCASE%newsessionrequest.h"

%API_CALLS_HEADER_INCLUDES%

I%API_NAME%::I%API_NAME%(QObject *parent)
    : QObject(parent)
{ }
void I%API_NAME%::initializeAndStart()
{
    //wait for business to become ready etc, then:
    emit readyForSessions();
}
void I%API_NAME%::newSession(I%API_NAME%NewSessionRequest *request)
{
    request->respond(new %API_NAME%Session(this));
}
%API_CALLS_DEFINITIONS%
%API_CALLS_RESPONSE_DEFINITIONS%
