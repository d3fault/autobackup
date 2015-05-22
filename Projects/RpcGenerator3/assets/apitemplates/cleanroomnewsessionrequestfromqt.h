#ifndef %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMQT_H
#define %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMQT_H

#include "i%API_NAME_LOWERCASE%newsessionrequest.h"

class %API_NAME%Session;

class %API_NAME%NewSessionRequestFromQt : public I%API_NAME%NewSessionRequest
{
    Q_OBJECT
public:
    %API_NAME%NewSessionRequestFromQt(%API_NAME% *cleanRoom, QObject *objectToRespondTo, const char *slotCallback, QObject *parent = 0)
        : I%API_NAME%NewSessionRequest(cleanRoom, parent)
        , m_ObjectToRespondTo(objectToRespondTo)
    {
        QObject::connect(this, SIGNAL(newSessionResponseRequested(%API_NAME%Session*)), objectToRespondTo, slotCallback);
    }
    void respond(%API_NAME%Session *session);
private:
    QObject *m_ObjectToRespondTo;
signals:
    void newSessionResponseRequested(%API_NAME%Session *session);
};

#endif // %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMQT_H
