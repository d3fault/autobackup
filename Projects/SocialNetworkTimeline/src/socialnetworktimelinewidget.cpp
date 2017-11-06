#include "socialnetworktimelinewidget.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>

SocialNetworkTimelineWidget::SocialNetworkTimelineWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout(this);
    myLayout->addWidget(m_DebugOutput = new QPlainTextEdit());
}
void SocialNetworkTimelineWidget::handleE(QString msg)
{
    m_DebugOutput->appendPlainText("error: " + msg + "\n");
}
void SocialNetworkTimelineWidget::handleO(QString msg)
{
    m_DebugOutput->appendPlainText(msg + "\n");
}
void SocialNetworkTimelineWidget::handleInitializeSocialNetworkTimelineFinished(bool success)
{
    if(success)
        handleO("initialize of StupidKeyValueContentTracker finished successfully");
    else
        handleE("initialization of StupidKeyValueContentTracker finished unsuccessfully");
}
void SocialNetworkTimelineWidget::handleAppendJsonObjectToSocialNetworkTimelineFinished(bool success)
{
    if(success)
        handleO("handleAppendJsonObjectToSocialNetworkTimelineFinished successfully");
    else
        handleO("handleAppendJsonObjectToSocialNetworkTimelineFinished unsuccessfully");
}
