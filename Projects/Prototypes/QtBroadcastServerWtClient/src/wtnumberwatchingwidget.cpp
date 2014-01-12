#include "wtnumberwatchingwidget.h"

WtNumberWatchingWidget::WtNumberWatchingWidget(const WEnvironment &env)
    : WApplication(env)
{
    m_Canvas = new WContainerWidget();
    new WText("Enter A Number To Watch For: ", m_Canvas);
    m_WatchNumberLineEdit = new WLineEdit(m_Canvas);
    m_WatchNumberLineEdit->enterPressed().connect(this, &WtNumberWatchingWidget::addNumberToWatch);
    WPushButton *pb = new WPushButton("Watch", m_Canvas);
    pb->clicked().connect(this, &WtNumberWatchingWidget::addNumberToWatch);
    m_Canvas->addWidget(new WBreak());
    root()->addWidget(m_Canvas);

    //"connect" to the qt broadcast server. it will WServer.post() to us
    QMetaObject::invokeMethod(QtBroadcastServerNumberWatcher::Instance(), "connectToQtBroadcastServer", Qt::QueuedConnection, Q_ARG(QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, this), Q_ARG(std::string, WApplication::instance()->sessionId()), Q_ARG(WtClientCallback, boost::bind(&WtNumberWatchingWidget::watchedNumberSeenUpdateGuiCallback, this, _1)));

    this->enableUpdates(true);
}
void WtNumberWatchingWidget::addNumberToWatch()
{
    bool ok = false;
    int num = QString::fromUtf8(m_WatchNumberLineEdit->text().toUTF8().c_str()).toInt(&ok);
    if(ok)
    {
        QMetaObject::invokeMethod(QtBroadcastServerNumberWatcher::Instance(), "watchForNum", Qt::QueuedConnection, Q_ARG(QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, this), Q_ARG(int, num));
    }
}
void WtNumberWatchingWidget::watchedNumberSeenUpdateGuiCallback(int number)
{
    //TODOreq: pretty sure we are a WApplication and in proper context so why would we check that we exist lol? Old prototype is old? app->triggerUpdate() can just be triggerUpdat()...
    WApplication *app = WApplication::instance();
    if(app)
    {
        m_Canvas->addWidget(new WText("Number Seen: " + WString::fromUTF8((const char*)(QString::number(number).toUtf8()))));
        m_Canvas->addWidget(new WBreak());

        app->triggerUpdate();
    }
}
void WtNumberWatchingWidget::finalize()
{
    QMetaObject::invokeMethod(QtBroadcastServerNumberWatcher::Instance(), "disconnectFromQtBroadcastServer", Qt::QueuedConnection, Q_ARG(QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, this));

    WApplication::finalize();
}
