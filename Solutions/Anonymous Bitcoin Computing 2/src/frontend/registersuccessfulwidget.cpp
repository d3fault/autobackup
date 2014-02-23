#include "registersuccessfulwidget.h"

#define ABC_REGISTER_SUCCESSFUL_MESSAGE_PREFIX "Welcome to Anonymous Bitcoin Computing, "
#define ABC_REGISTER_SUCCESSFUL_MESSAGE_SUFFIX ". You can now log in."

RegisterSuccessfulWidget::RegisterSuccessfulWidget(const std::string &username, WContainerWidget *parent)
    : WContainerWidget(parent), m_Message(new WText(ABC_REGISTER_SUCCESSFUL_MESSAGE_PREFIX + username + ABC_REGISTER_SUCCESSFUL_MESSAGE_SUFFIX, this))
{ }
void RegisterSuccessfulWidget::setUsername(const std::string &username)
{
    m_Message->setText(ABC_REGISTER_SUCCESSFUL_MESSAGE_PREFIX + username + ABC_REGISTER_SUCCESSFUL_MESSAGE_SUFFIX);
}
