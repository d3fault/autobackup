#ifndef WTAPP_H
#define WTAPP_H

#include "libqtwtcompat/WQApplication"
using namespace Wt;

#include "randomnumbersingleton.h"
#include "wtsessionspecificnumberwatcher.h"

class QtAwareWtApp : public WQApplication
{
public:
    QtAwareWtApp(const WEnvironment& env, WServer &server);

    virtual void create();
    virtual void destroy();
private:
    WLineEdit *m_WatchNumberWLineEdit;

    WtSessionSpecificNumberWatcher *m_MyNumberWatcher;
    RandomNumberSingleton *m_RandomNumberSingleton;

    WServer &m_WtServer;

    void watchedNumberSeenUpdateGuiCallback(const int &number);
};

#endif // WTAPP_H
