#ifndef TIMELINETAB_WIDGET_FORMAINMENUTABWIDGET_H
#define TIMELINETAB_WIDGET_FORMAINMENUTABWIDGET_H

#include "../iactivitytab_widget_formainmenutabwidget.h"

class TimelineTab_Widget_ForMainMenuTabWidget : public IActivityTab_Widget_ForMainMenuTabWidget
{
    Q_OBJECT
public:
    explicit TimelineTab_Widget_ForMainMenuTabWidget(QWidget *parent = 0);
    virtual MainMenuActivitiesEnum::MainMenuActivitiesEnumActual mainMenuActivityType() const;
};

#endif // TIMELINETAB_WIDGET_FORMAINMENUTABWIDGET_H
