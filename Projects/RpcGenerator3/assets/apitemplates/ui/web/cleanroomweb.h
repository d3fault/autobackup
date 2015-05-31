%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#include <Wt/WServer>
using namespace Wt;
#ifndef %API_NAME_UPPERCASE%WEB_H
#define %API_NAME_UPPERCASE%WEB_H

#include <QObject>
#include <QSharedPointer>

class %API_NAME%;

class %API_NAME%Web : public QObject
{
    Q_OBJECT
public:
    explicit %API_NAME%Web(int argc, char *argv[], QObject *parent = 0);
    virtual ~%API_NAME%Web();
private:
    %API_NAME% *m_%API_NAME%;
    QSharedPointer<WServer> m_WtServer;
signals:
    void initializeAndStartRequested();
private slots:
    void handle%API_NAME%ReadyForSessions();
};

#endif // %API_NAME_UPPERCASE%WEB_H
