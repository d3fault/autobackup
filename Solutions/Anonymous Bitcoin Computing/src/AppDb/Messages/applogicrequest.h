#ifndef APPLOGICREQUEST_H
#define APPLOGICREQUEST_H

#include "../../shared/WtFrontEndAndAppDbProtocol.h"

class AppLogicRequest
{
public:
    AppLogicRequest();
    static AppLogicRequest *fromWtFrontEndToAppDbMessage(WtFrontEndToAppDbMessage message);
};

#endif // APPLOGICREQUEST_H
