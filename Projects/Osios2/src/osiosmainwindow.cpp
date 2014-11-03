#include "osiosmainwindow.h"

#include <QTabWidget>
#include <QSettings>
#include <QColor>
#include <QDockWidget>
#include <QDebug>

#include "osiossettings.h"
#include "osiosnotificationswidget.h"
#include "iactivitytab_widget_formainmenutabwidget.h"
#include "mainmenuitems/timelinetab_widget_formainmenutabwidget.h"
#include "mainmenuitems/writertab_widget_formainmenutabwidget.h"

#include "timelinenodetypes/profilecreationannounce_aka_genesistimelinenode.h"
#include "timelinenodetypes/mainmenuactivitychangedtimelinenode.h"
#include "timelinenodetypes/keypressinnewemptydoctimelinenode.h"

/*TODOreq:
textEdit.append(newKeystroke); //synthesis
renderTextEditIntoQImageForSendingBack(&textEdit);
//^no race condition for the new keystroke to be in the image? methinks not, but idk for sure*/

//Still wrapping my head around this: 2 mainwindows hidden (mimicking + rendering + hashing-screencap-of the 2 neighbors' timeline node stream), 1 mainwindow shown. Do I also need multiple Osios backends to account for that?
OsiosMainWindow::OsiosMainWindow(Osios *osios, QWidget *parent)
    : QMainWindow(parent)
    , m_MainMenuItemsTabWidget(new QTabWidget())
    , m_CopycatModeEnabled(false) //TODOmb: make protected in ICopycatClient interface
{
    setWindowTitle(OSIOS_HUMAN_READABLE_TITLE);

    //all added tabs must implement IMainMenuActivityTab
    m_MainMenuItemsTabWidget->addTab(new TimelineTab_Widget_ForMainMenuTabWidget(osios), tr("Actions Timeline"));
    m_MainMenuItemsTabWidget->addTab(new WriterTab_Widget_ForMainMenuTabWidget(this, this), tr("Writer"));

    setCentralWidget(m_MainMenuItemsTabWidget);

    addDockWidgets();

    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());

    connect(m_MainMenuItemsTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleMainMenuItemsTabWidgetCurrentTabChanged()));
}
QObject *OsiosMainWindow::asQObject()
{
    return this;
}
void OsiosMainWindow::setCopycatModeEnabled(bool enabled)
{
    if(enabled != m_CopycatModeEnabled)
    {
        m_CopycatModeEnabled = enabled;

        if(enabled)
        {
            emit connectEventSynthesisSignalsForCopycattingRequested(this);
        }
        else
        {
            emit disconnectEventSynthesisSignalsForCopycattingRequested(this);
        }
    }
}
void OsiosMainWindow::closeEvent(QCloseEvent *event)
{
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}
void OsiosMainWindow::addDockWidgets()
{
    setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks);

    QDockWidget *notificationsPanelDockWidget = new QDockWidget(tr("Notifications"), this);
    notificationsPanelDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
    notificationsPanelDockWidget->setObjectName("Osios Notifications");

    OsiosNotificationsWidget *osiosNotificationsWidget = new OsiosNotificationsWidget();
    notificationsPanelDockWidget->setWidget(osiosNotificationsWidget);
    addDockWidget(Qt::BottomDockWidgetArea, notificationsPanelDockWidget, Qt::Horizontal);

    connect(this, SIGNAL(presentNotificationRequested(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)), osiosNotificationsWidget, SLOT(presentNotification(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)));
}
#if 0
void OsiosMainWindow::connectEventSynthesisSignalsForCopycatting()
{

}
void OsiosMainWindow::disconnectEventSynthesisSignalsForCopycatting()
{
    disconnect(this, SIGNAL(synthesizeKeyPressedInNewEmptyDoc(KeyPressInNewEmptyDocTimelineNode*)));
}
#endif
void OsiosMainWindow::changeConnectionColor(int color)
{
    //TODOreq:
    //temp:
    QColor myColor(static_cast<Qt::GlobalColor>(color));
    setWindowTitle(myColor.name());
}
void OsiosMainWindow::synthesizeEventUsingTimelineNodeReceivedFromCopycatTarget(OsiosDhtPeer *osiosDhtPeer /* TODOoptional: don't send backend peer to front-end xD */, TimelineNode timelineNode)
{
    //TODOreq:
    switch(timelineNode->TimelineNodeType)
    {
    case TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode:
    {
        //as an exception, the main menu actvitity tab change does not use signal/slot (just plain call to private method synthesizeMainMenuActivityTabChange), since this class is in charge of handling it
        synthesizeMainMenuActivityTabChange(osiosDhtPeer, static_cast<MainMenuActivityChangedTimelineNode*>(timelineNode));
    }
        break;
    case TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode:
    {
#if 0 //it's a thought, but...
        if(focusWidget()) //TODOreq: there may be many change focus network messages that i don't want to be a part of my timeline node stream (but i could change my mind and add them too idfk)
        {

        }
#endif
        emit synthesizeKeyPressedInNewEmptyDocRequested(osiosDhtPeer, static_cast<KeyPressInNewEmptyDocTimelineNode*>(timelineNode));
    }
        break;
    case TimelineNodeTypeEnum::ProfileCreationAnnounce_aka_GenesisTimelineNode:
    {
        //as of now, this timeline node event is only displayed in the timeline and has no other manifestation, so:
        //oops: timeline node is what i want emit presentNotificationRequested(static_cast<ProfileCreationAnnounce_aka_GenesisTimelineNode*>(timelineNode)->humanReadableShortDescriptionIncludingTimestamp());
        //hmm so i don't think i need to do anything actually, but i still need to catch it so as not to go to the default area

        QByteArray emptyByteArray; //TODoreq: see the similar use in when ProfileCreationAnnounce_aka_GenesisTimelineNode is actually created in osios.cpp
        emit timelineNodeAppliedAndRendered(osiosDhtPeer, timelineNode, emptyByteArray);
    }
        break;
    case TimelineNodeTypeEnum::INITIALNULLINVALIDTIMELINENODETYPE:
    default:
    {
        emit presentNotificationRequested(tr("While in copycat mode, the Osios front-end received from the back-end a timeline node of an unknown type: ") + QString::number(timelineNode->TimelineNodeType), OsiosNotificationLevels::WarningNotificationLevel);
    }
        return;
    }

#if 0 //lol. these screenshots are pretty damn accurate/with-latest-timeline-node-applied (could probably even be hashed themselves and matched as I used to want to do (now I'm going for combo strategy of 'machine verifiable' (sha1) and 'human verifiable' (rendered correctly) to achieve a level of certainty that gives me a fucking boner)). (DONE: the reason timeline view doesn't work is because it's sent "later" from the backend i think (there's just a lag of 1 timeline entry)). I almost want to give the screenshot stream it's own port/connection, so it doesn't clutter the crypto verify stream. really though, onl lan it won't matter much, and that's my primary target. TODOreq: if the status notification panel is moved "up" or undocked etc, the screenshots won't match. i probably shouldn't even take a screenshot of the status notifications panel. There is also the issue of resolution: they need to be exactly the same in order to match. The system themes being different could also throw the real-estate-available-for-widget-contents off by just a few pixels. If I could set a theme for a specific widget (it would reflect what the timeline node sender's theme is TODOoptional) and then render that widget into an image off screen, that would be neat. Hell maybe I can already xD, *tests*. Woot it does work, but the size is 640x480 and the un-used space is black. I have to either send size stuff to render (or widget.resize), or have to call show() first and it'll figure out the size itself. maybe two quick calls to show()/hide() won't look too stupid. Well it works, but yes it looks stupid. Fuck yea SIZE HINT saves the day <3 Qt. I should probably only render the central widget for comparison, not the entire main window (so the notification panel isn't rendered). HOWEVER, I could see actions in the file menus or toolbars being timeline nodes, so rendering central widget fails in that case (*cough sub main window as central widget of this main window side-steps that problem TODOmb?*.) TODOoptimization: [better] compression. can use lossy because the rendered results aren't being saved or compared, but only viewed and then discarded. WebP? I kind of want to keep separate the "copycat" and "copycat + take screenshot of self and send back to originator" functionality, to support "monitor clone" standalone mode, but I think it's obvious which one I'd rather have if I had to choose. TODOoptimization: [image] diffing between the previous frame could probably save us a TON of bandwidth. A keystroke for example modified only a tiny portion of the screen. BUT perhaps that is best left to a proper video encoder (VP9). Do I want to send a series of images, or a video stream? Video stream would probably be more efficient, and additionally could be "on it's own port/connection" with ease since libav has tons of streaming protocols built in. Also worth noting AGAIN (reminder) that I only want 1/3 of the screenshot (if possible, only render that 1/3 as well). 3 screens, 9 video streams, 3 timeline streams, 2 replicas, two security layers (computer, human). Maybe I should put the "this monitor" version of the widget (the main one I've pretty much already coded) on top of the recombined other 3 pieces, and use transparency on the "this monitor" version. Because that main menu does still need to have focus, but we want the recombined thirds to APPEAR as if they were on top. 50% transparency would be decent: you'd still be able to see the contents if the video streams died. It would appear to be not transparent at all during normal operation. You are able to notice when the 3rds streams fail. OT-ish but vidya got me thinkin: Obiviously I'd like to integrate "video frame" (B or I) == timeline node, but I wonder about it's efficiency (probably fine tbh). I also day dream on the idea of having a video game built into it, each video game action recorded/etc too. But seriously development tools integrated (d=) would be FUCKING SEX compared to those two high bandwidth examples just mentioned. One problem with VP9 is encode speed on my quad core ex-gaming machine: 1fps xD. Fuck it though, so long as it looks good enough I'd use theora idfc. I'll probably see the thirds stitching lines pretty apparently lolol (and could probably even fix them with blurring etc). For now I'm inclined to send PNGs to KISS. If I only send them on every timeline node, perhaps the bandwidth isn't an issue anyways (whereas video I can't control with such fine-grained accuracy)
    //takeScreenshot();
    QImage myScreenshot(size() /*or sizeHint() if the widget hasn't been shown() yet*/, QImage::Format_RGB32);
    QPainter myPainter(&myScreenshot);
    render(&myPainter);
    myScreenshot.save("/run/shm/screenAt" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png", "PNG");
#endif
}
//MAIN MENU ACTIVITY TAB CHANGE -- RECORD
void OsiosMainWindow::handleMainMenuItemsTabWidgetCurrentTabChanged()
{
    //pass through event (n/a, we're a signal sent after it already occured, not a virtual override like most similar 'record' methods)

    //now record event
    TimelineNode mainMenuActivityChangedTimelineNode(new MainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType()));
    //MainMenuActivityChangedTimelineNode *mainMenuActivityChangedTimelineNode = new MainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType());
    QByteArray mainMenuTabWidgetScreenshotHash = IOsiosClient::calculateCryptographicHashOfWidgetRenderedToImage(m_MainMenuItemsTabWidget);

    emit actionOccurred(mainMenuActivityChangedTimelineNode, mainMenuTabWidgetScreenshotHash);
}
//MAIN MENU ACTIVITY TAB CHANGE -- SYNTHESIZE
void OsiosMainWindow::synthesizeMainMenuActivityTabChange(OsiosDhtPeer *osiosDhtPeer, MainMenuActivityChangedTimelineNode *mainMenuTabChangedTimelineNode)
{
    //apply event
    //we don't know the tab index that it correlates with (right they match the enum, but maybe i'll allow tab re-ordering in the future), but we can find it
    int numTabs = m_MainMenuItemsTabWidget->count();
    bool tabFound = false;
    for(int i = 0; i < numTabs; ++i)
    {
        if(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->widget(i))->mainMenuActivityType() == mainMenuTabChangedTimelineNode->NewMainMenuActivity)
        {
            m_MainMenuItemsTabWidget->setCurrentIndex(i);
            tabFound = true;
            break;
        }
    }
    if(!tabFound)
    {
        emit presentNotificationRequested(tr("While in copycat mode, the Osios front-end received from the back-end a change main menu activity timeline node of an unknown type"), OsiosNotificationLevels::ErrorNotificationLevel);
        return;
    }

    //render for proofz
    QByteArray mainMenuTabWidgetScreenshotHash = IOsiosClient::calculateCryptographicHashOfWidgetRenderedToImage(m_MainMenuItemsTabWidget);

    emit timelineNodeAppliedAndRendered(osiosDhtPeer, mainMenuTabChangedTimelineNode, mainMenuTabWidgetScreenshotHash);
}
