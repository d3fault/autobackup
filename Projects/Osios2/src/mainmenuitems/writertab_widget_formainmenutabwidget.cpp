#include "writertab_widget_formainmenutabwidget.h"

#include <QVBoxLayout>
#include "writer/eventemittingplaintextedit.h"
#include "osiossettings.h"

WriterTab_Widget_ForMainMenuTabWidget::WriterTab_Widget_ForMainMenuTabWidget(IOsiosClient *osiosClient, IOsiosCopycatClient *copycatClient, QWidget *parent)
    : IActivityTab_Widget_ForMainMenuTabWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->setContentsMargins(OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS);

    myLayout->addWidget(new EventEmittingPlainTextEdit(osiosClient, copycatClient));
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
