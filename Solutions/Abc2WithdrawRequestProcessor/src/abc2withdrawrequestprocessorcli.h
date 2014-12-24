#ifndef ABC2WITHDRAWREQUESTPROCESSORCLI_H
#define ABC2WITHDRAWREQUESTPROCESSORCLI_H

#include <QObject>

#include <QTextStream>

#include "abc2withdrawrequestprocessor.h"

class Abc2WithdrawRequestProcessorCli : public QObject
{
    Q_OBJECT
public:
    explicit Abc2WithdrawRequestProcessorCli(QObject *parent = 0);
private:
    Abc2WithdrawRequestProcessor *m_WithdrawalRequestProcessor;
    QTextStream m_StdErr;
    QTextStream m_StdOut;
    QTextStream m_StdIn;
signals:
    void userHasSelectedAwayToHandleTheBitcoindCommunicationsError(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum userSelectedWayToHandleBitcoindCommunicationsError, const QString &errorStringToStoreInDb);
private slots:
    void handleE(const QString &msg);
    void handleO(const QString &msg);
    void handleCalculationIterationComplete_SoWaitForUserInputBeforeContinuingOntoExecutionIteration(const QString &calculatedAmount);
    void handleBitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(const QString &sourceOfError, const QString &theErrorItself);
};

#endif // ABC2WITHDRAWREQUESTPROCESSORCLI_H
