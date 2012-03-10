#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
    m_BitcoinD.setWorkingDirectory(PATH_TO_BITCOIND);
    connect(&m_BitcoinD, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleProcessError(QProcess::ProcessError)));
}
BitcoinHelper *BitcoinHelper::m_pInstance = NULL;
BitcoinHelper *BitcoinHelper::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new BitcoinHelper();
    }
    return m_pInstance;
}
QString BitcoinHelper::getNewKeyToReceivePaymentsAt()
{
    //no parsing of the result, since only the address is returned... so we just return whatever it returns
    return bitcoind("getnewaddress");

    //TODO: when throwing a key INTO bitcoind (doesn't apply to this method, but somewhere in this class in the future), we must sanitize our inputs. we could be vulnerable to something similar to an sql injection... since we're dealing with string commands. "string command injection" is better than sql injection TM COPYRIGHT 2012 just kidding fuck off
}
void BitcoinHelper::handleProcessError(QProcess::ProcessError processError)
{
    //todomb: we could save the full command in bitcoind() below as a QString as m_CurrentCommand and then if we get an error here, emit it as well. would probably help with debuggin... but i'm not having problems (yet)
    emit d("QProcess signal'd an error code: " + QString::number(processError));
}
QString BitcoinHelper::bitcoind(QString apiCmd, QString optionalApiCmdArg1, QString optionalApiCmdArg2)
{
    QStringList argList; //motherfuckin start() was adding quotes around my apiCmd that contained spaces (getreceivedbyaddress <key> <confirmations>)
    argList << BITCOIN_DATA_DIR;
    argList << apiCmd;
    if(!optionalApiCmdArg1.isEmpty())
    {
        argList << optionalApiCmdArg1;
        if(!optionalApiCmdArg2.isEmpty())
        {
            argList << optionalApiCmdArg2;
        }
    }
    m_BitcoinD.start(QString(PATH_TO_BITCOIND) + QString(BITCOIND_PROCESS), argList, QIODevice::ReadOnly);
    if(!m_BitcoinD.waitForStarted(5000)) //we WANT to block since we have our own thread just for interfacing with bitcoind. every call to bitcoind should go through this thread via an event
    {
        emit d("error: bitcoind process call never started");
        return QString();
    }
    if(!m_BitcoinD.waitForFinished())
    {
        emit d("error: bitcoind process call never finished");
        return QString();
        //todo: i suppose returning blank QString's is alright here.. but the caller should probably do some validation. however, i can predict the scenario where i check a key returned by this function against... you guessed it... this function. pretty sure there's an 'isvalidkey' command or someshit. if that too error'd out, there might be a possible case of an infinte loop (though i don't think so)
    }
    QByteArray result = m_BitcoinD.readAllStandardOutput();
    return QString(result).trimmed(); //pretty sure trim only removes leading/trailing whitespace/newlines. newline being my biggest concern atm. but some api commands return multiple results separated by a space... which need to be parsed by the caller. /hope .trimmed() doesn't mess with THAT whitespace..
}
double BitcoinHelper::parseAmountAtAddressForConfirmations(int confirmations, QString addressToCheck)
{
    QString bitcoindResult = bitcoind("getreceivedbyaddress", addressToCheck, QString::number(confirmations, 'f', 0)); //TODOreq: get lost in thought making sure that addressToCheck is never a user supplied address. as of my code so far, we're only using this method in the polling of addresses we created.. so it should be ok (famous last words, maybe i should sanitize it ANYWAYS). huge potential vulnerability here. HUGE. entire wallet lost huge. also, lol: don't use bitcoind to verify it's accuracy.. as that'd have the same pitfall. use some regular expressions or filtering or someshit

    if(bitcoindResult.isEmpty())
    {
        emit d("bitcoind result is empty");
        return 0.0;
    }
    double parsedAmount = 0.0;
    bool convertWentOk = false;
    parsedAmount = bitcoindResult.toDouble(&convertWentOk);
    if(!convertWentOk)
    {
        emit d("error parsing the double out of getreceivebyaddress: " + bitcoindResult);
        return 0.0;
    }
    return parsedAmount;
}
