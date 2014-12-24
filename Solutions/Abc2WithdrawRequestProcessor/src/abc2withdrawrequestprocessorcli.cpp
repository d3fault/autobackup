#include "abc2withdrawrequestprocessorcli.h"

#include <QCoreApplication>

//TODOoptimization: calculate-wait-for-user-input-then-execute mode. I let it sit at "wait for user input" mode until the calculated amount is received (with 1 confirmation) at the payout wallet. It saves me a view query, AND ALSO ENSURES THAT THE CALCULATED AMOUNT WILL BE THE SAME AS THE EXECUTED AMOUNT (currently executed amount might be more, which will trigger bitdoind error stuffs and then I'd do the "rever+stop" action). In fact perhaps this should be the default action, and then "execute-only" and "calculate-only" would be optional (and ofc "calculate-only" could also be chosen at the 'wait for user input stage' by typing...  q... for example)
//^An alternative to that is to optionally allow "execute" mode to rely on a view query that was persisted to disk (one withdrawal request key per line, or csv/tsv/whatever) -- optionally because it would still query the view if no file is provided. I "kind of" thought of this before but when I thought of it that time, I was considering doing all the db stuff up front and then taking the "list of payouts" to an offline box. This is different, but still uses a KIND OF "payout list"
/*
So best case of options would be this:
Default: calculate and wait for user input, re-use view query for execute when user says to (quittable still)
--calculate-only (don't wait for user input, just quit. spits out a total number at the end to stdout. saving the withdrawal request keys to file is another option)
--execute-only (don't calculate first, but does rely on view query)
--execute-only-from-calculated-file=/path/to/file/with/one/withdrawal/request/per/line
*/
//TODOreq: calculate only mode needs to completely avoid error recovery code
Abc2WithdrawRequestProcessorCli::Abc2WithdrawRequestProcessorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
    , m_StdIn(stdin)
{
    QStringList argz = QCoreApplication::arguments();
    if(argz.length() != 2)
    {
        handleE("You need to specify either --calculate-only or --execute");
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }

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
