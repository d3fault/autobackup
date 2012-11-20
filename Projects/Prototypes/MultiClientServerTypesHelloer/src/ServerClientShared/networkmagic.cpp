#include "networkmagic.h"

const quint8 NetworkMagic::m_MagicExpected[MAGIC_BYTE_SIZE] =  { 'F', 'U', 'C', 'K' };

NetworkMagic::NetworkMagic()
    : m_CurrentMagicByteIndex(0), m_MagicGot(false)
{ }
void NetworkMagic::setMagicGot(bool toSet)
{
    m_MagicGot = toSet;
}
bool NetworkMagic::needsMagic()
{
    return !m_MagicGot;
}
void NetworkMagic::streamOutMagic(QDataStream *ds)
{
    *ds << m_MagicExpected[0];
    *ds << m_MagicExpected[1];
    *ds << m_MagicExpected[2];
    *ds << m_MagicExpected[3];
}
