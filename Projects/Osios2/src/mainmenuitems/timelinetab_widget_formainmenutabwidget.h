#ifndef TIMELINETAB_WIDGET_FORMAINMENUTABWIDGET_H
#define TIMELINETAB_WIDGET_FORMAINMENUTABWIDGET_H

#include "../iactivitytab_widget_formainmenutabwidget.h"
#include "../iosiosclient.h"

class QListWidget;

class Osios;

class TimelineTab_Widget_ForMainMenuTabWidget : public IActivityTab_Widget_ForMainMenuTabWidget, public IOsiosClient /* relay client only */
{
    Q_OBJECT
public:
    explicit TimelineTab_Widget_ForMainMenuTabWidget(Osios *osios, IOsiosClient *osiosClient, QWidget *parent = 0);
    virtual MainMenuActivitiesEnum::MainMenuActivitiesEnumActual mainMenuActivityType() const;

    QObject *asQObject();
private:
    QListWidget *m_TimelineNodesList;

    void appendTimelineNodeToMyListWidget(TimelineNode action);
signals:
    void actionOccurred(TimelineNode action);
private slots:
    void handleTimelineNodeAdded(TimelineNode action);
};

#endif // TIMELINETAB_WIDGET_FORMAINMENUTABWIDGET_H
