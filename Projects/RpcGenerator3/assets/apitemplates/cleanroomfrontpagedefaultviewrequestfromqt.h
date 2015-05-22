#ifndef %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "i%API_NAME_LOWERCASE%frontpagedefaultviewrequest.h"

class %API_NAME%Session;

class %API_NAME%FrontPageDefaultViewRequestFromQt : public I%API_NAME%FrontPageDefaultViewRequest
{
    Q_OBJECT
public:
    %API_NAME%FrontPageDefaultViewRequestFromQt(%API_NAME% *cleanRoom, QObject *objectCallback, const char *slotCallback);
    void respond(QStringList frontPageDocs);
signals:
    void frontPageDefaultViewResponseRequested(QStringList frontPageDocs);
};

#endif // %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
