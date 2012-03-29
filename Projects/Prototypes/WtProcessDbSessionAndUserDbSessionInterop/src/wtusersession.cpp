#include "wtusersession.h"

WtUserSession::WtUserSession(const WEnvironment &env, WServer &server)
    : WApplication(env), m_Server(server)
{
}
void WtUserSession::create()
{
    m_RandomNumberSessionWatcher = new RandomNumberSessionWatcher();
}
void WtUserSession::destroy()
{
}
