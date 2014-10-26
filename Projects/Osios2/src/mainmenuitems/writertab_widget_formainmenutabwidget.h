#ifndef WRITERTAB_WIDGET_FORMAINMENUTABWIDGET_H
#define WRITERTAB_WIDGET_FORMAINMENUTABWIDGET_H

#include "../iactivitytab_widget_formainmenutabwidget.h"

class WriterTab_Widget_ForMainMenuTabWidget : public IActivityTab_Widget_ForMainMenuTabWidget
{
    Q_OBJECT
public:
    explicit WriterTab_Widget_ForMainMenuTabWidget(QWidget *parent = 0);
    virtual MainMenuActivitiesEnum::MainMenuActivitiesEnumActual mainMenuActivityType() const;
};

#endif // WRITERTAB_WIDGET_FORMAINMENUTABWIDGET_H
