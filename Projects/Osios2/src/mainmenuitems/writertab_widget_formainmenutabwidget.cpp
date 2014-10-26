#include "writertab_widget_formainmenutabwidget.h"

WriterTab_Widget_ForMainMenuTabWidget::WriterTab_Widget_ForMainMenuTabWidget(QWidget *parent)
    : IActivityTab_Widget_ForMainMenuTabWidget(parent)
{ }
MainMenuActivitiesEnum::MainMenuActivitiesEnumActual WriterTab_Widget_ForMainMenuTabWidget::mainMenuActivityType() const
{
    return MainMenuActivitiesEnum::WriterActivity;
}
