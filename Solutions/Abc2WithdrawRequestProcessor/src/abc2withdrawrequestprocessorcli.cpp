#include "abc2withdrawrequestprocessorcli.h"

#include <QCoreApplication>
#include <QHash>

//TODOoptimization: calculate-wait-for-user-input-then-execute mode. I let it sit at "wait for user input" mode until the calculated amount is received (with 1 confirmation) at the payout wallet. It saves me a view query, AND ALSO ENSURES THAT THE CALCULATED AMOUNT WILL BE THE SAME AS THE EXECUTED AMOUNT (currently executed amount might be more, which will trigger bitdoind error stuffs and then I'd do the "rever+stop" action). In fact perhaps this should be the default action, and then "execute-only" and "calculate-only" would be optional (and ofc "calculate-only" could also be chosen at the 'wait for user input stage' by typing...  q... for example)
//^An alternative to that is to optionally allow "execute" mode to rely on a view query that was persisted to disk (one withdrawal request key per line, or csv/tsv/whatever) -- optionally because it would still query the view if no file is provided. I "kind of" thought of this before but when I thought of it that time, I was considering doing all the db stuff up front and then taking the "list of payouts" to an offline box. This is different, but still uses a KIND OF "payout list"
/*
So best case of options would be this:
Default: calculate and wait for user input, re-use view query for execute when user says to (quittable still)
--calculate-only (don't wait for user input, just quit. spits out a total number at the end to stdout. saving the withdrawal request keys to file is another option)
--execute-only (don't calculate first, but does rely on view query)
--execute-only-from-calculated-file=/path/to/file/with/one/withdrawal/request/per/line
*/
//TODOreq: calculate only mode needs to completely avoid error recovery code. Maybe not actually, maybe it doesn't matter if I handle them during calculate or execute stage (would be a small TODOoptimization to not try to re-handle them during execute stage, if I did handle them already in calculate stage)). I'm thinking now actually that I NEED to do the error recovery code during calculate, because it tells me when a user shouldn't have any more of their withdrawal requests processed (and then my calculation might be off). Unsure tbh, too many implications everywhere
Abc2WithdrawRequestProcessorCli::Abc2WithdrawRequestProcessorCli(QObject *parent)
    : QObject(parent)
    , m_WithdrawalRequestProcessor(new Abc2WithdrawRequestProcessor(this))
    , m_StdErr(stderr)
    , m_StdOut(stdout)
    , m_StdIn(stdin)
{
    /* I think I'm going to implement 'calculate first, wait for user input, then execute' -- so 'execute-only' (better: 'dont-calculate') should be an option I implement early on
    QStringList argz = QCoreApplication::arguments();
    if(argz.length() != 2)
    {
        handleE("You need to specify either --calculate-only or --execute");
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    */

    connect(m_WithdrawalRequestProcessor, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(m_WithdrawalRequestProcessor, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(m_WithdrawalRequestProcessor, SIGNAL(calculationIterationComplete_SoWaitForUserInputBeforeContinuingOntoExecutionIteration(QString)), this, SLOT(handleCalculationIterationComplete_SoWaitForUserInputBeforeContinuingOntoExecutionIteration(QString)));
    connect(m_WithdrawalRequestProcessor, SIGNAL(bitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(QString,QString)), this, SLOT(handleBitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(QString,QString)));
    connect(this, SIGNAL(userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum,QString)), m_WithdrawalRequestProcessor, SLOT(userWantsUsToHandleTheBitcoindCommunicationsErrorThisWay(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum,QString)));
    connect(m_WithdrawalRequestProcessor, SIGNAL(withdrawalRequestProcessingFinished(bool)), qApp, SLOT(quit()), Qt::QueuedConnection);

    QMetaObject::invokeMethod(m_WithdrawalRequestProcessor, "processWithdrawalRequests", Qt::QueuedConnection);
}
void Abc2WithdrawRequestProcessorCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void Abc2WithdrawRequestProcessorCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void Abc2WithdrawRequestProcessorCli::handleCalculationIterationComplete_SoWaitForUserInputBeforeContinuingOntoExecutionIteration(const QString &calculatedAmount)
{
    QString userInput = "";
    QHash<QString /*user input number*/, QPair<QObject* /*receiver*/, QString /*slot in receiver*/> > userInputActionsHash;
    QPair<QObject*, QString> quitOption;
    quitOption.first = qApp;
    quitOption.second = "quit";
    userInputActionsHash.insert("1", quitOption);
    QPair<QObject*, QString> continueOption;
    continueOption.first = m_WithdrawalRequestProcessor;
    continueOption.second = "proceedOntoExecutionIteration";
    userInputActionsHash.insert("0", continueOption);
    while(!userInputActionsHash.contains(userInput))
    {
        m_StdOut << "Calcuating complete, so now is the time to transfer the appropriate amount to the payout wallet and wait for a confirmation." << endl;
        m_StdOut << "The amount to transfer to the payout wallet is: " << calculatedAmount << endl;
        m_StdOut << "Note: this does not take into account any balance already in the payout wallet, so you should run getbalance, and then subtract that from the above amount" << endl << endl; //TODOoptional: we can call getbalance ourselves...
        m_StdOut << "\t1 - Quit instead" << endl;
        m_StdOut << "\t0 - Proceed onto executing the payouts" << endl;
        userInput = m_StdIn.readLine();
    }
    const QPair<QObject*,QString> &selectedAction = userInputActionsHash.value(userInput);
    std::string selectedActionStdString = selectedAction.second.toStdString();
    QMetaObject::invokeMethod(selectedAction.first, selectedActionStdString.c_str(), Qt::QueuedConnection);
}
void Abc2WithdrawRequestProcessorCli::handleBitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(const QString &sourceOfError, const QString &theErrorItself)
{
    //TODOoptional: for types 1 and 3, we could do a "remember" type of functionality. When "remembering" for "3", we should probably only "remember" for that SPECIFIC type of bitcoin error (we'd build a "whitelist of errors to accept and continue"). Hell maybe even the whitelist would apply to option #1 as well. Obviously, extremely common 'remembered' types would become hardcoded in the source xD
    //ask the user how to proceed
    QString userInput = "";
    while(!(userInput == "1" || userInput == "2" || userInput == "3" || userInput == "4"))
    {
        handleE(sourceOfError);
        handleE(theErrorItself);
        m_StdOut << "Given the above error(s), how do you want to proceed?" << endl;
        m_StdOut << "\t1 - REVERT+CONTINUE (Set withdrawal request state back to unprocessed and unlock profile [without debitting])" << endl;
        m_StdOut << "\t2 - REVERT+STOP (Set withdrawal request state back to unprocessed and unlock profile [without debitting] and stop processing withdrawal requests (Quit))" << endl;
        m_StdOut << "\t3 - ACCEPT-ERROR+CONTINUE (Set withdrawal request state to bitcoindCommandReturnedError_Done and unlock profile [without debitting])" << endl;
        m_StdOut << "\t4 - ACCEPT-ERROR+STOP (Set withdrawal request state to bitcoindCommandReturnedError_Done and unlock profile [without debitting] and stop processing withdrawal requests (Quit))" << endl;
        userInput = m_StdIn.readLine();
    }
    QString errorStringToStoreInDb = sourceOfError + " -- " + theErrorItself;
    if(userInput == "1")
    {
        emit userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::RevertWithdrawalRequestStateToUnprocessedAndContinueProcessingWithdrawalRequests, errorStringToStoreInDb);
        return;
    }
    if(userInput == "2")
    {
        emit userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::RevertWithdrawalRequestStateToUnprocessedAndStopProcessingWithdrawalRequests, errorStringToStoreInDb);
        return;
    }
    if(userInput == "3")
    {
        emit userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::SetWithdrawalRequestStateToBitcoindReturnedErrorAndContinueProcessingWithdrawalRequests, errorStringToStoreInDb);
        return;
    }
    if(userInput == "4")
    {
        emit userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::SetWithdrawalRequestStateToBitcoindReturnedErrorAndStopProcessingWithdrawalRequests, errorStringToStoreInDb);
        return;
    }
}
