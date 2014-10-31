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

OsiosMainWindow::OsiosMainWindow(Osios *osios, QWidget *parent)
    : QMainWindow(parent)
    , m_MainMenuItemsTabWidget(new QTabWidget())
    , m_CopycatModeEnabled(false) //TODOmb: make protected in ICopycatClient interface
{
    setWindowTitle(tr("OSiOS"));

    //all added tabs must implement IMainMenuActivityTab
    m_MainMenuItemsTabWidget->addTab(new TimelineTab_Widget_ForMainMenuTabWidget(osios, this), tr("Actions Timeline"));
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
void OsiosMainWindow::synthesizeEventUsingTimelineNodeReceivedFromCopycatTarget(TimelineNode timelineNode)
{
    //TODOreq:
    switch(timelineNode->TimelineNodeType)
    {
    case TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode:
    {
        //as an exception, the main menu actvitity tab change does not use signal/slot (just plain call to private method synthesizeMainMenuActivityTabChange), since this class is in charge of handling it
        synthesizeMainMenuActivityTabChange(static_cast<MainMenuActivityChangedTimelineNode*>(timelineNode));
    }
        break;
    case TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode:
    {
#if 0 //it's a thought, but...
        if(focusWidget()) //TODOreq: there may be many change focus network messages that i don't want to be a part of my timeline node stream (but i could change my mind and add them too idfk)
        {

        }
#endif
        emit synthesizeKeyPressedInNewEmptyDocRequested(static_cast<KeyPressInNewEmptyDocTimelineNode*>(timelineNode));
    }
        break;
    case TimelineNodeTypeEnum::ProfileCreationAnnounce_aka_GenesisTimelineNode:
    {
        //as of now, this timeline node event is only displayed in the timeline and has no other manifestation, so:
        //oops: timeline node is what i want emit presentNotificationRequested(static_cast<ProfileCreationAnnounce_aka_GenesisTimelineNode*>(timelineNode)->humanReadableShortDescriptionIncludingTimestamp());
        //hmm so i don't think i need to do anything actually, but i still need to catch it
    }
        break;
    case TimelineNodeTypeEnum::INITIALNULLINVALIDTIMELINENODETYPE:
    default:
    {
        emit presentNotificationRequested(tr("While in copycat mode, the Osios front-end received from the back-end a timeline node of an unknown type: ") + QString::number(timelineNode->TimelineNodeType), OsiosNotificationLevels::WarningNotificationLevel);
    }
        break;
    }
}
//MAIN MENU ACTIVITY TAB CHANGE -- RECORD
void OsiosMainWindow::handleMainMenuItemsTabWidgetCurrentTabChanged()
{
    TimelineNode mainMenuActivityChangedTimelineNode(new MainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType()));
    //MainMenuActivityChangedTimelineNode *mainMenuActivityChangedTimelineNode = new MainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType());
    emit actionOccurred(mainMenuActivityChangedTimelineNode); //my listener should put that bitch in a shared pointer. copies can still be made by derefing and COW is still used, including the data still being valid (if copied before the last pointer destructs ofc). Correction: my listener receives that bitch in a shared pointer already :-P
}
//MAIN MENU ACTIVITY TAB CHANGE -- SYNTHESIZE
void OsiosMainWindow::synthesizeMainMenuActivityTabChange(MainMenuActivityChangedTimelineNode *mainMenuTabChangedTimelineNode)
{
    //we don't know the tab index that it correlates with (right they match the enum, but maybe i'll allow tab re-ordering in the future), but we can find it
    int numTabs = m_MainMenuItemsTabWidget->count();
    for(int i = 0; i < numTabs; ++i)
    {
        if(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->widget(i))->mainMenuActivityType() == mainMenuTabChangedTimelineNode->NewMainMenuActivity)
        {
            m_MainMenuItemsTabWidget->setCurrentIndex(i);
            return;
        }
    }
    emit presentNotificationRequested(tr("While in copycat mode, the Osios front-end received from the back-end a change main menu activity timeline node of an unknown type"), OsiosNotificationLevels::ErrorNotificationLevel);
}
