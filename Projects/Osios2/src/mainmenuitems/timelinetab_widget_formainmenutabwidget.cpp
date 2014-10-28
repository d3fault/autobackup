#include "timelinetab_widget_formainmenutabwidget.h"

#include <QVBoxLayout>
#include <QListWidget>

#include "../osios.h"

TimelineTab_Widget_ForMainMenuTabWidget::TimelineTab_Widget_ForMainMenuTabWidget(Osios *osios, IOsiosClient *osiosClient, QWidget *parent)
    : IActivityTab_Widget_ForMainMenuTabWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->setContentsMargins(OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS);

    m_TimelineNodesList = new QListWidget();
    Q_FOREACH(TimelineNode currentTimelineNode, osios->timelineNodes())
    {
        appendTimelineNodeToMyListWidget(currentTimelineNode);
    }
    myLayout->addWidget(m_TimelineNodesList);

    setLayout(myLayout);

    connect(this, SIGNAL(actionOccurred(TimelineNode)), osiosClient->asQObject(), SIGNAL(actionOccurred(TimelineNode)));
    connect(osios, SIGNAL(timelineNodeAdded(TimelineNode)), this, SLOT(handleTimelineNodeAdded(TimelineNode)));
}
MainMenuActivitiesEnum::MainMenuActivitiesEnumActual TimelineTab_Widget_ForMainMenuTabWidget::mainMenuActivityType() const
{
    return MainMenuActivitiesEnum::TimelineViewActivity;
}
QObject *TimelineTab_Widget_ForMainMenuTabWidget::asQObject()
{
    return this;
}
void TimelineTab_Widget_ForMainMenuTabWidget::appendTimelineNodeToMyListWidget(TimelineNode action)
{
    QListWidgetItem *currentListWidgetItem = new QListWidgetItem(action->humanReadableShortDescriptionIncludingTimestamp(), m_TimelineNodesList);
    currentListWidgetItem->setData(Qt::UserRole, QVariant::fromValue(action));
}
void TimelineTab_Widget_ForMainMenuTabWidget::handleTimelineNodeAdded(TimelineNode action)
{
    appendTimelineNodeToMyListWidget(action);
}
