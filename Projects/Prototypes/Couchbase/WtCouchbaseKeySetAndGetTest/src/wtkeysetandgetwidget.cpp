#include "wtkeysetandgetwidget.h"

message_queue *WtKeySetAndGetWidget::m_KeyMessageQueue = NULL;
message_queue *WtKeySetAndGetWidget::m_ValueMessageQueue = NULL;
event *WtKeySetAndGetWidget::m_SetValueByKeyEvent = NULL;

WtKeySetAndGetWidget::WtKeySetAndGetWidget(const WEnvironment& env)
    : WApplication(env)
{
    m_Canvas = new WContainerWidget();
    new WText("Enter A Key/Value To Set:", m_Canvas);
    m_KeyLineEdit = new WLineEdit(m_Canvas);
    m_ValueLineEdit = new WLineEdit(m_Canvas);
    m_KeyLineEdit->enterPressed().connect(this, &WtKeySetAndGetWidget::setValueByKey);
    m_ValueLineEdit->enterPressed().connect(this, &WtKeySetAndGetWidget::setValueByKey);
    WPushButton *pb = new WPushButton("Set", m_Canvas);
    pb->clicked().connect(this, &WtKeySetAndGetWidget::setValueByKey);
    m_Canvas->addWidget(new WBreak());
    root()->addWidget(m_Canvas);

    //"connect" to the qt broadcast server. it will WServer.post() to us
    //QMetaObject::invokeMethod(QtBroadcastServerNumberWatcher::Instance(), "connectToQtBroadcastServer", Qt::QueuedConnection, Q_ARG(QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, this), Q_ARG(std::string, WApplication::instance()->sessionId()), Q_ARG(WtClientCallback, boost::bind(&WtNumberWatchingWidget::watchedNumberSeenUpdateGuiCallback, this, _1)));

    this->enableUpdates(true);
}
void WtKeySetAndGetWidget::newAndOpenSetValueByKeyMessageQueues(const char *keyMessageQueueName, const char *valueMessageQueueName)
{
    m_KeyMessageQueue = new message_queue(open_only, keyMessageQueueName);
    m_ValueMessageQueue = new message_queue(open_only, valueMessageQueueName);
}
void WtKeySetAndGetWidget::deleteSetValueByKeyMessageQueues()
{
    delete m_KeyMessageQueue;
    delete m_ValueMessageQueue;
}
void WtKeySetAndGetWidget::setValueByKey()
{
    //idk how i'm supposed to pass the values to the event. a mutex [protecting a queue (might write multiple times before the couchbase thread even has-time/attempts-to lock the mutex and process the values (and it would be an optimization even if it only happened with queues of size 2 in length))] comes to mind, but i'm unable to determine whether or not that would ruin the design/scalability...
    //^^For now i just want to see if i can even get Wt and Couchbase to even play nicely. Fuck the values~
    //^^^Or perhaps wt can do one of those "reader" locks where any time the couchbase thread tries to lock the mutex, the couchbase thread takes precedence (aside from a reader already locked). read/write doesn't fit exactly what i'm trying to say (because we still can't have multiple readers at once), but I know there's something like this in boost's libs...

    m_KeyMessageQueue->send(m_KeyLineEdit->text().toUTF8().c_str(), strlen(m_KeyLineEdit->text().toUTF8().c_str()), 0);
    m_ValueMessageQueue->send(m_ValueLineEdit->text().toUTF8().c_str(), strlen(m_ValueLineEdit->text().toUTF8().c_str()), 0);
    event_active(m_SetValueByKeyEvent, EV_READ|EV_WRITE, 0); //might be able to take EV_WRITE out of that flag, but idfk i don't even get what they're  for so yea leaving for now. Might be the opposite, maybe I can take out EV_READ (since the callback is waiting for read and i would be writing here? idfk)
}
void WtKeySetAndGetWidget::finalize()
{
    WApplication::finalize();
}
