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

    if(environment().ajax())
    {
        this->enableUpdates(true);
    }

    //TODOreq: doesn't belong here, but after logging in (since i'm not using Wt's auth shit), I should call this apparently (it's doc says to)...
    //this->changeSessionId();
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
    SetValueByKeyRequestFromWt setValueByKeyRequestFromWt(sessionId(), this, m_KeyLineEdit->text().toUTF8(), m_ValueLineEdit->text().toUTF8());

    {
        boost::archive::text_oarchive serializer(setValueByKeyRequestFromWtSerialized);
        serializer << setValueByKeyRequestFromWt;
    }

    std::string setValueByKeyRequestFromWtSerializedString = setValueByKeyRequestFromWtSerialized.str();

    //TODOreq: ensure serialized buffer not over maximum message size

    m_SetValueByKeyRequestFromWtMessageQueue->send(setValueByKeyRequestFromWtSerializedString.c_str(), setValueByKeyRequestFromWtSerializedString.length(), 0); //omg it rhymes <3
    //i pronounce queue "kwee/qwee" <3. fuck your cue shit
    event_active(m_SetValueByKeyRequestFromWtEvent, EV_READ|EV_WRITE, 0); //might be able to take EV_WRITE out of that flag, but idfk i don't even get what they're  for so yea leaving for now. Might be the opposite, maybe I can take out EV_READ (since the callback is waiting for read and i would be writing here? idfk)

    //OT'ish ramblings: to say "javascript-less" clients should synchronously use lcb is one thing, and indeed i could add a boost wait condition here and hacky logic to the approapriate lcb callbacks and a bool saying javascriptless and the wait condition itself (to wake) inside SetValueByKeyRequestFromWt that's passed around.... BUT really isn't that pretty fucking stupid to block here? I'm pretty sure asio (what Wt uses) is not multi-threaded but in fact asynchronous! Meaning if I block here then no other WApplications can be served. So bleh I'd never thought I'd mutter these words: javascript is more efficient. javascript is only more efficient because it allows you to continue with other requests while a separate thread interacts with the db. If there was a way in Wt to say "done for now but don't even start responding so we can respond later, because they're dolts (or d3faults) and don't have javascript enabled", then the statement about javascript being more efficient would be false. So I'm inclined to say it's Wt's fault... but maybe it isn't and I'm just dumb. Hey come to think of it I can say that "or" after any statement at all.... or maybe I can't and I'm just dumb.
    //^^perhaps WApplication::deferRendering and resumeRendering is what I want for javascript-less async? I'm pretty sure this just disables the GUI [using javascript no less] and doesn't "hold off on responding in general" like I want. Should be easy to test. basically subtitute enableUpdates with deferRendering, and triggerUpdates with resumeRendering... depending on "bool ajax()". note that resumeRendering would go before the m_Canvas->addWidget calls in the callback, not after them like triggerUpdate
    //^^woot defer/resume does just that! javascript-less solved! fucken love Wt!
    if(!environment().ajax())
    {
        //should maybe use this even when ajax is enabled to keep them from going to another page before we can respond... but really nothing can stop them from closing the browser etc anyways. maybe all my "updates" could be in a small console in the corner that is on every page? so if they do browse away after an "Add" (before we can WServer::post the response), they still see the result of an action initiated on a previous page. In that case, then no we shouldn't enable deferRendering for ajax clients :)
        deferRendering();
    }

    //WHAT THE FUCK Wt _IS_ multi-threaded? As the old saying goes, "all my assumptions are/were wrong". Fuck fuck fuck fuck fuck. I was so close to being at a point where I could actually start coding the implementation (requiring javascript (re:above) is fine idgaf). Despite [re(?)-]realizing Wt is multi-threaded, I'm _STILL_ inclined to believe that only a single WApplication instance can be "active" at any one time. I fucking remember reading that man, and hell even their PostGreSQL example wouldn't have been MT safe if that wasn't true (then again, it scaled like shit anyways...). Oh cool you can turn Wt threading off, but you lose the exotic feature of server push -_-... fuck fuck fuck fuck fuck. All my dreams last night (or I guess I should say all day :-P) consisted of trying to solve the HILARIOUS problem of passing a fucking class pointer accross a thread in a platform (32/64) independent way. I also realized how fucking WEIRD it is that the Qt prototype worked to begin with. Boost::bind makes a copy of the WApplication::blahCallback instance method (Wt's docs say to do it, so eh not surprised it works)... then QMetaObject::invokeMethod makes ANOTHER copy... and finally the WServer::post is able to get back to the original instance!?!? does it ignore the callback aside from "which one it is" and use the sessionId to pull the original instance out of some hash/etc?
    //I could have sworn I read somewhere that Wt is async using asio and that only one WApplication is active at a time. Maybe the "multi threading" is for all the other stuff like "handling sending/receiving" (pre/post WApplication stuffs) but it is still true that only one WApplication is only ever active on one thread at at time (async style). Need to dig into this now fuuuuck, going backwards...
    //One one hand, Wt being multi-threaded (in that multiple WApplications can be active at once) is a good thing when it comes to dealing with javascript-less clients (a few here and there block? no biggy). On the other, fffffffffffffffffffff idfk if my use of message_queue and/or libevent::event_active are "thread safe" (obviously sending/receiving on the message_queue is thread safe, that's the whole purpose of it... but what about two threads trying to send at the same time!?!?!? guh). And then there's still the fact that I haven't figured out how to send the WApplication pointer/callback around... but I have a feeling that'll be easy once I figure it out (everything is).
    //NOPE: Ahah more shit saying I am right about only one WApplication instance active at once: this "UpdateLock" shit that you CAN grab from other threads. It says that the lock is already being held. Aww shit is it a member variable (per-WApplication)? So much for that -_-

    //this->rawUniqueId();
    //so err ehh uhm hack mode engaged, this is the best thing I can think of: Wt uses thread pool of size 10, so I either srand(rawUniqueId) or rawUniqueId % 10 (OR REALLY ANY 0-9 random number) to give me a random selector between 10x mutexes,message_queues,event_actives (thought i'd need 10x wait conditions too, but am wrong). i try_lock the randomly selected mutex. if say i was trying to lock mutex 4 and it failed, i go on to try_lock 5, 6, 7... all the way back to 3. on mutex 3 if all the others have failed, i do a BLOCKING lock so it will work eventually. whichever lock i get, i write to the corresponding message_queue and event_active the corresponding event (there are 10 of each). On the couchbase side of things I don't need to do any locking at all. Ideally I could use a macro/good-code to set my "10" to the same size as Wt's thread pool... but I can't find it programmatically (looking for that, or a thread id, is how i found rawUniqueId (and also that OT changeSessionId() thing too wtf)) so fuck it.
    //while searching "lock" on /libraries/ on boost to make sure i'm doing try_lock right, i discover "boost::lockfree::queue". WTF where have you been all my life baybee?
    //Hahaha it basicall does exactly what I was going to do manually, but automatically (no need to specify "10" anywhere) _AND_ faster if the hardware supports atomics (most do these days). Committing before trying because idk I have a bad feeling about it still (and I want to try more experiments with getting that damn pointer accross xD)
    //Woot, managed_shared_memory::handle_t serializes... and eh I wanna finish that tangent off before diving into lockfree queue (which should be easy anyways)...
#if 0 //scratch pad below, leaving for lulz
    managed_shared_memory segment(create_only, "blah", 20480000); //20mb just for pointers to WApplications should be plenty, i hope...
    void *pointerToMemoryChunkLargeEnoughToStoreAddressOfPointerToWApplication = segment.allocate(sizeof(WApplication*));
    //write the address
    //memset(pointerToMemoryChunkLargeEnoughToStoreAddressOfPointerToWApplication, this, sizeof(WApplication*));
    //*pointerToMemoryChunkLargeEnoughToStoreAddressOfPointerToWApplication = this; //write the address into the address pointed to by the pointer!?!? my head hurts (IT'S ONLY A SEGFAULT ON MY VM IF I AM WRONG SO WHO CARES TRIAL AND ERROR FTW)
    memcpy(pointerToMemoryChunkLargeEnoughToStoreAddressOfPointerToWApplication, static_cast<WApplication*>(this), sizeof(WApplication*)); //uhh, writing &this feels dirty :-P...

    managed_shared_memory::handle_t handleLoL = segment.get_handle_from_address(pointerToMemoryChunkLargeEnoughToStoreAddressOfPointerToWApplication);

    //handle: serialize, lockfree::queue, event_active, deserialize

    void *sameFuckingLocation = segment.get_address_from_handle(handleLoL);
    WApplication *lolSameFuckingOneButWhoKnowsCouldBeDeletedxDThxWServerPostForBeinSmartAsFuck = static_cast<WApplication*>(sameFuckingLocation);
    //then boost::bind as normal in WServer::post() params <3

    //BTW since I'm using this handle_t shit, I should serialize into that. err wait no then how do i send a handle to myself? xD. wtf is so great about a handle_t if you have to serialize it into a void* to get it over to the other thread? why not just send the........ DOH
    //... i guess i can message_queue::send the WApplication* cast'd to a void*... i'm retarded... but then i'd be using my "many message queue" design again, which is shit, AND didn't/doesn't account for the multi-threaded nature of Wt. So in order to serialize a void* into a single "message/parameter object", i need to use that handle_t shit. Fuck it, at this point i'll just be happy if it works
#endif
    //damn, no boost::lockfree in wheezy repositories :-/. What to choose... so many choices ^_>. Why the fuck is passing pointers around so frowned upon in the boost world? I do it all the damn time in Qt :)... ok fuck that i solved that problem, back to this multi-producer-single-consumer problem (ez)...
}
void WtKeySetAndGetWidget::valueSetByKeyCallback(std::string key, std::string value)
{
    if(!environment().ajax())
    {
        resumeRendering();
    }

    m_Canvas->addWidget(new WText("Key \"" + WString::fromUTF8(key) + "\" has been ADDED with value \"" + WString::fromUTF8(value) + "\""));
    m_Canvas->addWidget(new WBreak());

    if(environment().ajax())
    {
        triggerUpdate();
    }
}
void WtKeySetAndGetWidget::finalize()
{
    WApplication::finalize();
}
