#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromqt.h"

%API_NAME%FrontPageDefaultViewRequestFromQt::%API_NAME%FrontPageDefaultViewRequestFromQt(%API_NAME% *cleanRoom, QObject *objectCallback, const char *slotCallback)
    : I%API_NAME%FrontPageDefaultViewRequest(cleanRoom)
{
    QObject::connect(this, SIGNAL(frontPageDefaultViewResponseRequested(QStringList)), objectCallback, slotCallback);
}
void %API_NAME%FrontPageDefaultViewRequestFromQt::respond(QStringList frontPageDocs)
{
    emit frontPageDefaultViewResponseRequested(frontPageDocs);
}
