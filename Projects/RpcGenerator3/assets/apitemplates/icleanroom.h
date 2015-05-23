#ifndef I%API_NAME_UPPERCASE%_H
#define I%API_NAME_UPPERCASE%_H

#include <QObject>

class I%API_NAME%NewSessionRequest;
%API_CALLS_FORWARD_DECLARATIONS%

class I%API_NAME% : public QObject
{
    Q_OBJECT
public:
    explicit I%API_NAME%(QObject *parent = 0);
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();
    void newSession(I%API_NAME%NewSessionRequest *request);

%API_CALLS_DECLARATIONS%

%API_CALL_RESPONSES_DECLARATIONS%
};

#endif // I%API_NAME_UPPERCASE%_H
