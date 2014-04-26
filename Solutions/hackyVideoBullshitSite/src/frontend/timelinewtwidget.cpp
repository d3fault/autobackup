#include "timelinewtwidget.h"

#include <Wt/WGroupBox>
#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WBreak>

//I feel guilty for enjoying coding this. It's not like I've been, you know, putting off coding it for... well over 3 years... (MusicTimeline became ...)
TimeLineWtWidget::TimeLineWtWidget(const QString &myOwnInternalPath, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_MyOwnInternalPath(myOwnInternalPath)
    , m_CurrentDirection(Horizontal)
{
    WGroupBox *timeLineDirectionGroupBox = new WGroupBox("TimeLine Direction", this);
    WButtonGroup *timeLineDirectionButtonGroup = new WButtonGroup(timeLineDirectionGroupBox);
    WRadioButton *button = new WRadioButton("Horizontal", timeLineDirectionGroupBox);
    timeLineDirectionButtonGroup->addButton(button, Horizontal);
    button = new WRadioButton("Vertical", timeLineDirectionGroupBox);
    timeLineDirectionButtonGroup->addButton(button, Vertical);
    timeLineDirectionButtonGroup->setCheckedButton(timeLineDirectionButtonGroup->button(m_CurrentDirection));
    new WBreak(this);

    timeLineDirectionButtonGroup->checkedChanged().connect(this, &TimeLineWtWidget::handleDirectionRadiosCheckChanged); //TODOreq: works without javascript? never know with the web, it might be a form button so yes...
}
void TimeLineWtWidget::handleInternalPathChanged(const QString &newInternalPath)
{
    if(newInternalPath == m_MyOwnInternalPath)
    {
        //maybe let them pick a starting point? or should we default to one of the ends?

        return;
    }

    //we've already QDir::clean'd it and established that it starts with our internal path, but the timeline internal path has not been chopped off yet...
    //QString
}
void TimeLineWtWidget::handleDirectionRadiosCheckChanged(WRadioButton *newlyCheckedRadioButton)
{
    TimeLineDirection newDirection = static_cast<TimeLineDirection>(newlyCheckedRadioButton->group()->id(newlyCheckedRadioButton)); //stupid, just pass the id mr. api!
    if(newDirection == m_CurrentDirection)
        return;

    m_CurrentDirection = newDirection;

}
