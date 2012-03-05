#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper(QObject *parent) :
    QObject(parent)
{
}
QString BitcoinHelper::getNewKeyToReceivePaymentsAt()
{
    //todo: send a command to bitcoind process 'getnewaddress' or whatever.. parse the output and then return the QString here

    return QString("ABCXYZTHISISAKEY123");
}
