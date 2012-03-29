#ifndef WTUSERSESSION_H
#define WTUSERSESSION_H

#include "../../../../tps/libqtwtcompat/WQApplication"
using namespace Wt;

class WtUserSession : public WQApplication
{
public:
    WtUserSession(const WEnvironment& env, WServer &server);

    virtual void create();
    virtual void destroy();
private:
    WServer &m_Server;
};

#endif // WTUSERSESSION_H
