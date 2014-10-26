#ifndef WRITERTAB_WIDGET_FORMAINMENUTABWIDGET_H
#define WRITERTAB_WIDGET_FORMAINMENUTABWIDGET_H

#include "../iactivitytab_widget_formainmenutabwidget.h"

#include "../iosiosclient.h"

class WriterTab_Widget_ForMainMenuTabWidget : public IActivityTab_Widget_ForMainMenuTabWidget, public IOsiosClient /* relay client only */
{
    Q_OBJECT
public:
    explicit WriterTab_Widget_ForMainMenuTabWidget(IOsiosClient *osiosClient, QWidget *parent = 0);
    virtual MainMenuActivitiesEnum::MainMenuActivitiesEnumActual mainMenuActivityType() const;
    QObject *asQObject();
signals:
    void actionOccurred(TimelineNode action);
};

#endif // WRITERTAB_WIDGET_FORMAINMENUTABWIDGET_H
