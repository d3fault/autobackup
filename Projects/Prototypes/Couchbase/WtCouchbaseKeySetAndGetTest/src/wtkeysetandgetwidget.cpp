#include "wtkeysetandgetwidget.h"

#include "setvaluebykeyrequestfromwt.h"

message_queue *WtKeySetAndGetWidget::m_SetValueByKeyRequestFromWtMessageQueue = NULL;
event *WtKeySetAndGetWidget::m_SetValueByKeyRequestFromWtEvent = NULL;

WtKeySetAndGetWidget::WtKeySetAndGetWidget(const WEnvironment& env)
    : WApplication(env)
{
    m_Canvas = new WContainerWidget();
    new WText("Enter A Key/Value To Add: ", m_Canvas);
    m_KeyLineEdit = new WLineEdit(m_Canvas);
    m_ValueLineEdit = new WLineEdit(m_Canvas);
    m_KeyLineEdit->enterPressed().connect(this, &WtKeySetAndGetWidget::setValueByKey);
    m_ValueLineEdit->enterPressed().connect(this, &WtKeySetAndGetWidget::setValueByKey);
    WPushButton *pb = new WPushButton("Add", m_Canvas);
    pb->clicked().connect(this, &WtKeySetAndGetWidget::setValueByKey);
    m_Canvas->addWidget(new WBreak());
    root()->addWidget(m_Canvas);

    //"connect" to the qt broadcast server. it will WServer.post() to us
    //QMetaObject::invokeMethod(QtBroadcastServerNumberWatcher::Instance(), "connectToQtBroadcastServer", Qt::QueuedConnection, Q_ARG(QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, this), Q_ARG(std::string, WApplication::instance()->sessionId()), Q_ARG(WtClientCallback, boost::bind(&WtNumberWatchingWidget::watchedNumberSeenUpdateGuiCallback, this, _1)));

    this->enableUpdates(true);
}
void WtKeySetAndGetWidget::newAndOpenSetValueByKeyMessageQueue(const char *keyMessageQueueName)
{
    m_SetValueByKeyRequestFromWtMessageQueue = new message_queue(open_only, keyMessageQueueName);
}
void WtKeySetAndGetWidget::deleteSetValueByKeyMessageQueue()
{
    delete m_SetValueByKeyRequestFromWtMessageQueue;
}
void WtKeySetAndGetWidget::setValueByKey()
{
    //TODOreq: sanitize of course

    //idk how i'm supposed to pass the values to the event. a mutex [protecting a queue (might write multiple times before the couchbase thread even has-time/attempts-to lock the mutex and process the values (and it would be an optimization even if it only happened with queues of size 2 in length))] comes to mind, but i'm unable to determine whether or not that would ruin the design/scalability...
    //^^For now i just want to see if i can even get Wt and Couchbase to even play nicely. Fuck the values~
    //^^^Or perhaps wt can do one of those "reader" locks where any time the couchbase thread tries to lock the mutex, the couchbase thread takes precedence (aside from a reader already locked). read/write doesn't fit exactly what i'm trying to say (because we still can't have multiple readers at once), but I know there's something like this in boost's libs...

    std::ostringstream setValueByKeyRequestFromWtSerialized;
    //damn I took serializing a class method for granted in Qt. I miss thee, Qt. Actually shit I was just looking for that the other day and Qt doesn't have it. boost::bind is what is doing the real work. So what the fuck... together they can serialize a class method for callback, but independently they both can't. Great. Or maybe I was just getting lucky with my hacky Qt/boost::bind callback and the C++ gods would frown upon the way I used it?
    SetValueByKeyRequestFromWt setValueByKeyRequestFromWt(sessionId(), m_KeyLineEdit->text().toUTF8(), m_ValueLineEdit->text().toUTF8(), boost::bind(&WtKeySetAndGetWidget::valueSetByKeyCallback, this, _2));

    {
        boost::archive::text_oarchive serializer(setValueByKeyRequestFromWtSerialized);
        serializer << setValueByKeyRequestFromWt;
    }

    const char *setValueByKeyRequestFromWtSerializedBuffer = setValueByKeyRequestFromWtSerialized.str().c_str();

    //TODOreq: ensure serialized buffer not over maximum message size

    m_SetValueByKeyRequestFromWtMessageQueue->send(setValueByKeyRequestFromWtSerializedBuffer, strlen(setValueByKeyRequestFromWtSerializedBuffer), 0); //omg it rhymes <3
    event_active(m_SetValueByKeyRequestFromWtEvent, EV_READ|EV_WRITE, 0); //might be able to take EV_WRITE out of that flag, but idfk i don't even get what they're  for so yea leaving for now. Might be the opposite, maybe I can take out EV_READ (since the callback is waiting for read and i would be writing here? idfk)

    //OT'ish ramblings: to say "javascript-less" clients should synchronously use lcb is one thing, and indeed i could add a boost wait condition here and hacky logic to the approapriate lcb callbacks and a bool saying javascriptless and the wait condition itself (to wake) inside SetValueByKeyRequestFromWt that's passed around.... BUT really isn't that pretty fucking stupid to block here? I'm pretty sure asio (what Wt uses) is not multi-threaded but in fact asynchronous! Meaning if I block here then no other WApplications can be served. So bleh I'd never thought I'd mutter these words: javascript is more efficient. javascript is only more efficient because it allows you to continue with other requests while a separate thread interacts with the db. If there was a way in Wt to say "done for now but don't hang-up/finalize the connection because they're dolts (or d3faults) and don't have javascript enabled", then the statement about javascript being more efficient would be false. So I'm inclined to say it's Wt's fault... but maybe it isn't and I'm just dumb. Hey come to think of it I can say that "or" after any statement at all.... or maybe I can't and I'm just dumb.
}
void WtKeySetAndGetWidget::valueSetByKeyCallback(std::string key, std::string value)
{
    m_Canvas->addWidget(new WText("Key \"" + WString::fromUTF8(key) + "\" has been ADDED with value \"" + WString::fromUTF8(value) + "\""));
    m_Canvas->addWidget(new WBreak());
    triggerUpdate();
}
void WtKeySetAndGetWidget::finalize()
{
    WApplication::finalize();
}
