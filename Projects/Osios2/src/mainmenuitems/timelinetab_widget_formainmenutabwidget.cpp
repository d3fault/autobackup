#include "timelinetab_widget_formainmenutabwidget.h"

TimelineTab_Widget_ForMainMenuTabWidget::TimelineTab_Widget_ForMainMenuTabWidget(QWidget *parent)
    : IActivityTab_Widget_ForMainMenuTabWidget(parent)
{ }
MainMenuActivitiesEnum::MainMenuActivitiesEnumActual TimelineTab_Widget_ForMainMenuTabWidget::mainMenuActivityType() const
{
    return MainMenuActivitiesEnum::TimelineViewActivity;
}
