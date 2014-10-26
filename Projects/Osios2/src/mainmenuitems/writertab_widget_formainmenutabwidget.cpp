#include "writertab_widget_formainmenutabwidget.h"

#include <QVBoxLayout>
#include "writer/eventemittingplaintextedit.h"

WriterTab_Widget_ForMainMenuTabWidget::WriterTab_Widget_ForMainMenuTabWidget(IOsiosClient *osiosClient, QWidget *parent)
    : IActivityTab_Widget_ForMainMenuTabWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->addWidget(new EventEmittingPlainTextEdit(osiosClient));
    setLayout(myLayout);

    connect(this, SIGNAL(actionOccurred(TimelineNode)), osiosClient->asQObject(), SIGNAL(actionOccurred(TimelineNode)));
}
MainMenuActivitiesEnum::MainMenuActivitiesEnumActual WriterTab_Widget_ForMainMenuTabWidget::mainMenuActivityType() const
{
    return MainMenuActivitiesEnum::WriterActivity;
}
QObject *WriterTab_Widget_ForMainMenuTabWidget::asQObject()
{
    return this;
}
