#ifndef I%API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUEST_H
#define I%API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUEST_H

#include <QObject>
#include "i%API_NAME_LOWERCASE%request.h"

#include "%API_NAME_LOWERCASE%.h"

class I%API_NAME%FrontPageDefaultViewRequest : public QObject, public I%API_NAME%Request
{
    Q_OBJECT
public:
    I%API_NAME%FrontPageDefaultViewRequest(%API_NAME% *cleanRoom, QObject *parent = 0)
        : QObject(parent)
    {
        connect(this, &I%API_NAME%FrontPageDefaultViewRequest::frontPageDefaultViewRequested, cleanRoom, &%API_NAME%::getFrontPageDefaultView);
    }
    void processRequest();
    virtual void respond(QStringList frontPageDocs)=0;
signals:
    void frontPageDefaultViewRequested(I%API_NAME%FrontPageDefaultViewRequest *request);
};

#endif // I%API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUEST_H
