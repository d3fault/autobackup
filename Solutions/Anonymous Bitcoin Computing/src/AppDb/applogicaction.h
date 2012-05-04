#ifndef APPLOGICACTION_H
#define APPLOGICACTION_H

#include <abcapplogic.h>

class AppLogicAction
{
public:
    static AbcAppLogic* appLogic();
    static void setAppLogic(AbcAppLogic *appLogic);

    virtual bool raceCondition1detected()=0;
    virtual bool raceCondition2detected()=0;
    virtual bool needsToCommunicateWithBankServer()=0;

    virtual BankServerAction *buildBankRequest(); //unsure if this is Action or Request (will responses be in same object?)

    virtual void performAction()=0;

    void setCurrentMessage(WtFrontEndAndAppDbMessage *message);
    void addCurrentMessageToPending();
private:
    static AbcAppLogic *m_AppLogic;

    QList<WtFrontEndAndAppDbMessage*> m_PendingActionsOfThisType; //named because the list is specific to whoever inherits it... to speed up checking of race condition #2, we only check for pending actions of the same type (TODOreq: think this through... that we don't have to check other pending actions of another type)
    WtFrontEndAndAppDbMessage *m_CurrentMessage;
};

#endif // APPLOGICACTION_H
