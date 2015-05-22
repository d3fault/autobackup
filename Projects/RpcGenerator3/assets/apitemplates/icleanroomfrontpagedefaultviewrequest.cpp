#include "i%API_NAME_LOWERCASE%frontpagedefaultviewrequest.h"

void I%API_NAME%FrontPageDefaultViewRequest::processRequest()
{
    emit frontPageDefaultViewRequested(this);
}
