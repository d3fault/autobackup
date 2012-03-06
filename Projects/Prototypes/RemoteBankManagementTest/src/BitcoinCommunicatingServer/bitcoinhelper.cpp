#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper(QObject *parent) :
    QObject(parent)
{
    m_BitcoinD = new QProcess();
    connect(m_BitcoinD, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleProcessError(QProcess::ProcessError)));
}
QString BitcoinHelper::getNewKeyToReceivePaymentsAt()
{
    //todo: send a command to bitcoind process 'getnewaddress' or whatever.. parse the output and then return the QString here

    m_BitcoinD->start(PATH_TO_BITCOIND, QStringList() << "getnewaddress", QIODevice::ReadOnly);
    if(!m_BitcoinD->waitForStarted(5000))
    {
        //todo: handle error
        return QString("invalid-key-never-started");
    }
    if(!m_BitcoinD->waitForFinished())
    {
        //todo: handle error
        return QString("invalid-key-never-finished");
    }
    QByteArray result = m_BitcoinD->readAllStandardOutput();
    return QString(result);
}
void BitcoinHelper::handleProcessError(QProcess::ProcessError processError)
{
    //todo: show the enum to debug
}
