#include "QtAwareWtApp.h"

QtAwareWtApp::QtAwareWtApp(const WEnvironment &env, WServer &server)
    : WQApplication(env, true), m_WtServer(server)
{
}
void QtAwareWtApp::create()
{
    setTitle("Multi-Client Number Watcher/Notifier");

    root()->addWidget(new WText("Num 0-10 to watch: "));
    m_WatchNumberWLineEdit = new WLineEdit(root());
    m_WatchNumberWLineEdit->setFocus();

    WPushButton *b = new WPushButton("Watch For It", root());
    b->setMargin(5, Left);

    b->clicked().connect(this, &QtAwareWtApp::propagateGreet);
    m_WatchNumberWLineEdit->enterPressed().connect(this, &QtAwareWtApp::propagateGreet);

    m_RandomNumberSingleton = RandomNumberSingleton::Instance();
    m_MyNumberWatcher = new WtSessionSpecificNumberWatcher(WApplication::instance()->sessionId(), m_WtServer, boost::bind(&QtAwareWtApp::watchedNumberSeenUpdateGuiCallback, this, _1));

    QObject::connect(m_RandomNumberSingleton, SIGNAL(numberGenerated(int)), m_MyNumberWatcher, SLOT(handleGeneratedNumber(int)));

    enableUpdates(true);
}
void QtAwareWtApp::destroy()
{
    QObject::disconnect(m_RandomNumberSingleton, 0, m_MyNumberWatcher, 0);
    delete m_MyNumberWatcher;
}
void QtAwareWtApp::watchedNumberSeenUpdateGuiCallback(int number)
{
    WApplication *app = WApplication::instance();
    if(app)
    {
        new WBreak(app->root());
        new WText("Number Seen: " + toWString(QString::number(number)), app->root());
        app->triggerUpdate();
    }
}
