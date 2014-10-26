#include "timelinetab_widget_formainmenutabwidget.h"

#include <QVBoxLayout>
#include <QListWidget>

#include "../osios.h"

TimelineTab_Widget_ForMainMenuTabWidget::TimelineTab_Widget_ForMainMenuTabWidget(Osios *osios, IOsiosClient *osiosClient, QWidget *parent)
    : IActivityTab_Widget_ForMainMenuTabWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QListWidget *m_TimelineNodesList = new QListWidget();
    Q_FOREACH(TimelineNode currentTimelineNode, osios->timelineNodes())
    {
        QListWidgetItem *currentListWidgetItem = new QListWidgetItem(currentTimelineNode->humanReadableShortDescription(), m_TimelineNodesList);
        currentListWidgetItem->setData(Qt::UserRole, QVariant::fromValue(currentTimelineNode));
    }
    myLayout->addWidget(m_TimelineNodesList);

    setLayout(myLayout);

    connect(this, SIGNAL(actionOccurred(TimelineNode)), osiosClient->asQObject(), SIGNAL(actionOccurred(TimelineNode)));
}
MainMenuActivitiesEnum::MainMenuActivitiesEnumActual TimelineTab_Widget_ForMainMenuTabWidget::mainMenuActivityType() const
{
    return MainMenuActivitiesEnum::TimelineViewActivity;
}
QObject *TimelineTab_Widget_ForMainMenuTabWidget::asQObject()
{
    return this;
}
