#ifndef WTUSERSESSION_H
#define WTUSERSESSION_H

#include <Wt/WApplication>
using namespace Wt;

class WtUserSession : public WApplication
{
public:
    WtUserSession(const WEnvironment& env);

    virtual void create();
    virtual void destroy();
};

#endif // WTUSERSESSION_H
