#ifndef WTAPP_H
#define WTAPP_H

#include "libqtwtcompat/WQApplication"
#include <Wt/WServer>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WBreak>
#include <Wt/WPushButton>
using namespace Wt;

#include "randomnumbersingleton.h"
#include "wtsessionspecificnumberwatcher.h"

class QtAwareWtApp : public WQApplication
{
public:
    QtAwareWtApp(const WEnvironment& env, WServer &server);

    void updateGuiAfterContextSwitch(int number);

    virtual void create();
    virtual void destroy();
private:
    std::string m_SessionId;
    WLineEdit *m_WatchNumberWLineEdit;

    WtSessionSpecificNumberWatcher *m_MyNumberWatcher;
    RandomNumberSingleton *m_RandomNumberSingleton;

    WServer &m_WtServer;

    void addEnteredNumberToWatch();
    void watchedNumberSeenUpdateGuiCallback(int number);
};

#endif // WTAPP_H
