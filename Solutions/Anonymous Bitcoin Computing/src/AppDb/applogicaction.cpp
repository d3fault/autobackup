#include "applogicaction.h"

AbcAppLogic* AppLogicAction::m_AppLogic = NULL;
void AppLogicAction::setAppLogic(AbcAppLogic *appLogic)
{
    m_AppLogic = appLogic;
}
AbcAppLogic * AppLogicAction::appLogic()
{
    return m_AppLogic;
}
BankServerAction *AppLogicAction::buildBankRequest()
{
    return 0;
}
void AppLogicAction::addCurrentMessageToPending()
{
    m_PendingActionsOfThisType.append(m_CurrentMessage);
}
void AppLogicAction::setCurrentMessage(WtFrontEndAndAppDbMessage *message)
{
    m_CurrentMessage = message;
}
