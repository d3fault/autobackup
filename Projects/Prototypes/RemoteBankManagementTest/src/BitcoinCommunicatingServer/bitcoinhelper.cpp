#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper(QObject *parent) :
    QObject(parent)
{
    m_BitcoinD = new QProcess();
    m_BitcoinD->setWorkingDirectory(PATH_TO_BITCOIND);
    connect(m_BitcoinD, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleProcessError(QProcess::ProcessError)));
}
QString BitcoinHelper::getNewKeyToReceivePaymentsAt()
{
    //no parsing of the result, since only the address is returned... so we just return whatever it returns
    return bitcoind("getnewaddress");
}
void BitcoinHelper::handleProcessError(QProcess::ProcessError processError)
{
    //todo: show the enum to debug
}
QString BitcoinHelper::bitcoind(QString apiCmd)
{
    m_BitcoinD->start(QString(PATH_TO_BITCOIND) + QString(BITCOIND_PROCESS), QStringList() << BITCOIN_DATA_DIR << apiCmd, QIODevice::ReadOnly);
    if(!m_BitcoinD->waitForStarted(5000))
    {
        //todo: handle error
        return QString("error-bitcoind-call-never-started");
    }
    if(!m_BitcoinD->waitForFinished())
    {
        //todo: handle error
        return QString("error-bitcoind-never-finished");
    }
    QByteArray result = m_BitcoinD->readAllStandardOutput();
    return QString(result).trimmed(); //pretty sure trim only removes leading/trailing whitespace/newlines. newline being my biggest concern atm. but some api commands return multiple results separated by a space... which need to be parsed by the caller. /hope .trimmed() doesn't mess with THAT whitespace..
}
