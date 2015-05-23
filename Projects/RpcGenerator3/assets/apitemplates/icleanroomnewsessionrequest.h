#ifndef I%API_NAME_UPPERCASE%NEWSESSIONREQUEST
#define I%API_NAME_UPPERCASE%NEWSESSIONREQUEST

#include <QObject>
#include "i%API_NAME_LOWERCASE%request.h"

#include "%API_NAME_LOWERCASE%.h"

class %API_NAME%Session;

class I%API_NAME%NewSessionRequest : public QObject, public I%API_NAME%Request
{
    Q_OBJECT
public:
    I%API_NAME%NewSessionRequest(I%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *parent = 0)
        : QObject(parent)
    {
        connect(this, &I%API_NAME%NewSessionRequest::newSessionRequested, %API_AS_VARIABLE_NAME%, &I%API_NAME%::newSession);
    }
    void processRequest()
    {
        emit newSessionRequested(this);
    }
    virtual void respond(%API_NAME%Session *newSession)=0;
signals:
    void newSessionRequested(I%API_NAME%NewSessionRequest *newSessionRequest);
};

#endif // I%API_NAME_UPPERCASE%NEWSESSIONREQUEST
