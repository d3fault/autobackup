#include "osiosmainwindow.h"

#include <QTabWidget>
#include <QSettings>
#include <QColor>
#include <QDockWidget>
#include <QDebug>

#include "osiosnotificationswidget.h"
#include "iactivitytab_widget_formainmenutabwidget.h"
#include "mainmenuitems/timelinetab_widget_formainmenutabwidget.h"
#include "mainmenuitems/writertab_widget_formainmenutabwidget.h"

#include "timelinenodetypes/mainmenuactivitychangedtimelinenode.h"

OsiosMainWindow::OsiosMainWindow(Osios *osios, QWidget *parent)
    : QMainWindow(parent)
    , m_MainMenuItemsTabWidget(new QTabWidget())
{
    setWindowTitle(tr("OSiOS"));

    //all added tabs must implement IMainMenuActivityTab
    m_MainMenuItemsTabWidget->addTab(new TimelineTab_Widget_ForMainMenuTabWidget(osios, this), tr("Timeline"));
    m_MainMenuItemsTabWidget->addTab(new WriterTab_Widget_ForMainMenuTabWidget(this), tr("Writer"));

    setCentralWidget(m_MainMenuItemsTabWidget);

    addDockWidgets();

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    connect(m_MainMenuItemsTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleMainMenuItemsTabWidgetCurrentTabChanged()));
}
QObject *OsiosMainWindow::asQObject()
{
    return this;
}
void OsiosMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}
void OsiosMainWindow::addDockWidgets()
{
    setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks);

    QDockWidget *notificationsPanelDockWidget = new QDockWidget(tr("Notifications"), this);
    notificationsPanelDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);

    OsiosNotificationsWidget *osiosNotificationsWidget = new OsiosNotificationsWidget();
    notificationsPanelDockWidget->setWidget(osiosNotificationsWidget);
    addDockWidget(Qt::BottomDockWidgetArea, notificationsPanelDockWidget, Qt::Horizontal);

    connect(this, SIGNAL(presentNotificationRequested(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)), osiosNotificationsWidget, SLOT(presentNotification(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)));
}
void OsiosMainWindow::changeConnectionColor(int color)
{
    //TODOreq:
    //temp:
    QColor myColor(static_cast<Qt::GlobalColor>(color));
    setWindowTitle(myColor.name());
}
void OsiosMainWindow::handleMainMenuItemsTabWidgetCurrentTabChanged()
{
    TimelineNode mainMenuActivityChangedTimelineNode(new MainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType()));
    //MainMenuActivityChangedTimelineNode *mainMenuActivityChangedTimelineNode = new MainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType());
    emit actionOccurred(mainMenuActivityChangedTimelineNode); //my listener should put that bitch in a shared pointer. copies can still be made by derefing and COW is still used, including the data still being valid (if copied before the last pointer destructs ofc). Correction: my listener receives that bitch in a shared pointer already :-P
}
