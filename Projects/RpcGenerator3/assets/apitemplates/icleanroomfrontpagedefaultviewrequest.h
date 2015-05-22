#ifndef I%API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUEST_H
#define I%API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUEST_H

#include <QObject>
#include "i%API_NAME_LOWERCASE%request.h"

#include "%API_NAME_LOWERCASE%.h"

class I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME% : public QObject, public I%API_NAME%Request
{
    Q_OBJECT
public:
    I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%(%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *parent = 0)
        : QObject(parent)
    {
        connect(this, &I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%::frontPageDefaultViewRequested, %API_AS_VARIABLE_NAME%, &%API_NAME%::getFrontPageDefaultView);
    }
    void processRequest();
    virtual void respond(QStringList frontPageDocs)=0;
signals:
    void frontPageDefaultViewRequested(I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME% *request);
};

#endif // I%API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUEST_H
