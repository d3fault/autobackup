#include "abc2withdrawrequestprocessorcli.h"

#include <QCoreApplication>

Abc2WithdrawRequestProcessorCli::Abc2WithdrawRequestProcessorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
    , m_StdIn(stdin)
{
    Abc2WithdrawRequestProcessor *processor = new Abc2WithdrawRequestProcessor(this);

    connect(processor, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(processor, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(processor, SIGNAL(bitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(QString,QString)), this, SLOT(handleBitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(QString,QString)));
    connect(this, SIGNAL(userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum,QString)), processor, SLOT(userWantsUsToHandleTheBitcoindCommunicationsErrorThisWay(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum,QString)));

    connect(processor, SIGNAL(withdrawalRequestProcessingFinished(bool)), qApp, SLOT(quit()), Qt::QueuedConnection);

    QMetaObject::invokeMethod(processor, "processWithdrawalRequests", Qt::QueuedConnection);
}
void Abc2WithdrawRequestProcessorCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void Abc2WithdrawRequestProcessorCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void Abc2WithdrawRequestProcessorCli::handleBitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(const QString &sourceOfError, const QString &theErrorItself)
{
    //TODOoptional: for types 1 and 3, we could do a "remember" type of functionality. When "remembering" for "3", we should probably only "remember" for that SPECIFIC type of bitcoin error (we'd build a "whitelist of errors to accept and continue"). Hell maybe even the whitelist would apply to option #1 as well. Obviously, extremely common 'remembered' types would become hardcoded in the source xD
    handleE(sourceOfError);
    handleE(theErrorItself);
    //ask the user how to proceed
    QString userInput = "";
    while(!(userInput == "1" || userInput == "2" || userInput == "3" || userInput == "4"))
    {
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
