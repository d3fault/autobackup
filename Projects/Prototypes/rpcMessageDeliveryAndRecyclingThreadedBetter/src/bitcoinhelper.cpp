#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
}
void BitcoinHelper::init()
{
    emit initialized();
}
void BitcoinHelper::start()
{
    emit started();
}
void BitcoinHelper::stop()
{
    emit stopped();
}
