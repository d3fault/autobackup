#include "QtAwareWtApp.h"

QtAwareWtApp::QtAwareWtApp(const WEnvironment &env, WServer &server)
    : WQApplication(env, true), m_WtServer(server)
{
}
void QtAwareWtApp::create()
{
    m_SessionId = WApplication::instance()->sessionId();

    setTitle("Multi-Client Number Watcher/Notifier");

    root()->addWidget(new WText("Num 0-10 to watch: "));
    m_WatchNumberWLineEdit = new WLineEdit(root());
    m_WatchNumberWLineEdit->setFocus();

    WPushButton *b = new WPushButton("Watch For It", root());
    b->setMargin(5, Left);

    //if i connect this Wt pushbutton to a method in this class, and then QMetaInvoke the new number to watch to the class, I may be able to remove the QMutex used in the random number watcher. it might require adding Qt::QueuedConnection to the connect() call below, but i doubt it
    b->clicked().connect(this, &QtAwareWtApp::addEnteredNumberToWatch);
    m_WatchNumberWLineEdit->enterPressed().connect(this, &QtAwareWtApp::addEnteredNumberToWatch);

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
void QtAwareWtApp::updateGuiAfterContextSwitch(int number)
{
    m_WtServer.post(m_SessionId, boost::bind(boost::bind(&QtAwareWtApp::watchedNumberSeenUpdateGuiCallback, this, _1), number));
}
void QtAwareWtApp::addEnteredNumberToWatch()
{
    //metainvoke here?
    QString numberAsString(toQString(m_WatchNumberWLineEdit->text()));
    bool ok = false;
    int num = numberAsString.toInt(&ok);
    if(ok)
    {
        m_MyNumberWatcher->watchNumber(num);
    }
}
