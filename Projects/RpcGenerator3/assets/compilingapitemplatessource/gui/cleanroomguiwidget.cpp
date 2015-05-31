#include "cleanroomguiwidget.h"

#include <QVBoxLayout>
#include <QLabel>

CleanRoomGuiWidget::CleanRoomGuiWidget(ICleanRoom *cleanRoom, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    for(int i = 0; i < NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE; ++i)
    {
        QLabel *newLabel = new QLabel();
        myLayout->addWidget(newLabel);
        m_CleanRoomDocsWidgets.insert(i, newLabel);
    }

    setLayout(myLayout);

    setDisabled(true); //TODOmb: whenever there's a pending request, we should disable the gui (deferRendering in Wt land). Atm 2 requests in a row, before the 1st one has responded, is undefined. I could define it, hence the 'mb' in that t0d0. There are certainly cases where N requests should be dispatched before a single response is received (unable to think of any atm, but no matter). HOWEVER: any api call that mutates the [implicitly shared] session [in the business object/thread] would then introduce a race condition should the system allow/define "multiple requests without first waiting for response". For example, a login api call calls Session::setLoggedIn(true) in the business object/thread. The session detaches when that happens, and the copy-of/reference-to the session that the front-end has still has loggedIn set to false. As you can see, allowing multiple api calls in a row (ie, NOT disabling the gui during a request/before-the-response) would lead to problems with my implicitly shared session design
    //^Elaborating-on/exploring the 'multiple requests before waiting for response'. Obvioiusly, a call to 'login' and then a call to 'post' before login has responded wouldn't work. There's the session detaching problem, but I mean also thread/contexts can take a while to switch anyways so that's another problem. One solution I'm just now entertaining, though, is an api call request queue. That way, successive calls to login and post WOULD work as expected. Bleh idk how the implementation of such a queue would work, fuck it I wrote down the idea good nuff. For now and to KISS: 1 request, 1 response, disable GUI (the ability to make more requests) in between

    CleanRoomSession::requestNewSession(cleanRoom, this, SLOT(handleNewSessionCreated(CleanRoomSession)));
}
void CleanRoomGuiWidget::handleNewSessionCreated(CleanRoomSession session)
{
    setDisabled(false);
    m_Session.reset(new CleanRoomSession(session)); //we do a 'new' here because we want to +1 the implicitly shared session (notice we're using the copy constructor of CleanRoomSession). If we did "&session" instead, then the pointer in the m_Session scoped pointer would be a dangling pointer. I had to use a scoped pointer around CleanRoomSession in the first place because CleanRoomSession has no default constructor
    m_Session->requestNewCleanRoomFrontPageDefaultView(this, SLOT(handleFrontPageDefaultViewReceived(QStringList)), 69420);
    setDisabled(true);
}
void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    //qDebug() << "front end received: " << frontPageDocs;
    //TODOreq: ez
    int max = NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE;
    int numDocs = qMin(frontPageDocs.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        m_CleanRoomDocsWidgets.at(i)->setText(frontPageDocs.at(i));
    }
    setDisabled(false);
}
