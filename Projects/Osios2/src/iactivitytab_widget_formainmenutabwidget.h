#ifndef IACTIVITYTAB_WIDGET_FORMAINMENUTABWIDGET_H
#define IACTIVITYTAB_WIDGET_FORMAINMENUTABWIDGET_H

#include <QWidget>

#include "osioscommon.h"

class IActivityTab_Widget_ForMainMenuTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IActivityTab_Widget_ForMainMenuTabWidget(QWidget *parent = 0);
    virtual MainMenuActivitiesEnum::MainMenuActivitiesEnumActual mainMenuActivityType() const=0;
};

#endif // IACTIVITYTAB_WIDGET_FORMAINMENUTABWIDGET_H
