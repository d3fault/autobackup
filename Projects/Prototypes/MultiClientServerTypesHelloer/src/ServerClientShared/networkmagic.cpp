#include "networkmagic.h"

const quint8 NetworkMagic::MagicExpected[MAGIC_BYTE_SIZE] =  { 'F', 'U', 'C', 'K' };
//i don't get it, am i doing this right? how is 'F' 'U' 'C' 'K' 32 bits and so is an md5sum (which is 32 DIGITS!). i guess it's all about encoding. ASCII vs. Hex. each of those are 8-bit, whereas hex is... 4? so how the fuck do you get 32 hex DIGITS?? wtf? 4*32 = 128... is md5 128-bit? pretty sure it's 32-bit. what am i missing here. just seems strange is all. hmm i think i've forgotten how hex works. base 16, but does that have anything to do with how many bits it is? I THINK IT'S 4 bitch! wtf!?!?!?
//just wikipedia'd that shit. MD5 _IS_ 128-bit... WHAT THE FUCK? guh 32 hexadecimal characters * 4 = 128. idiot. i guess i see where i got confused: 32 hex digits vs. 32 bits. lmfao. guess my cookie/clientId needs to be bigger too, fml. also, is magic big enough at 32-bits? 4 ascii characters? what are the chances that ciphertext will spit out MAGIC on accident (we'd also have to be 'out-of-sync' with messages... which should NEVER happen anyways (but robust coding means we should allow for it and recover from it))
//tl;dr: you are a noob, noob

NetworkMagic::NetworkMagic()
    : m_CurrentMagicByteIndex(0), m_MagicGot(false)
{ }
void NetworkMagic::streamOutMagic(QDataStream *ds)
{
    *ds << MagicExpected[0];
    *ds << MagicExpected[1];
    *ds << MagicExpected[2];
    *ds << MagicExpected[3];
}
bool NetworkMagic::consumeFromIODeviceByteByByteLookingForMagic_And_ReturnTrueIf__Seen_or_PreviouslySeen__And_FalseIf_RanOutOfDataBeforeSeeingMagic(QIODevice *deviceToLookForMagicOn)
{
    if(m_MagicGot)
    {
        //magic remains 'got' until we consume a whole message. this might take multiple iterations of readyRead being emitted, depending entirely on the size of the message. this bool is so we don't go looking for magic every time readyRead is emitted, if we didn't previously get a message after seeing magic (because there weren't enough bytesAvailable() to get it)
        return true;
    }
    //else: look for magic. returning false here (or just returning m_MagicGot) is a rookie mistake


    quint8 potentialMagicPiece;
    m_MagicReadingNetworkStream.setDevice(deviceToLookForMagicOn); //not sure how slow this function is, but a TODOoptimization would be to put all of this shit in a macro within the same 'business' or something or other idfk my brain hurts but it is probably possible to not make it have to be set every time. we might need MORE datastreams, but eh fuck it

    while(deviceToLookForMagicOn->bytesAvailable() > 0)
    {
        m_MagicReadingNetworkStream >> potentialMagicPiece;

        if(NetworkMagic::MagicExpected[m_CurrentMagicByteIndex] == potentialMagicPiece)
        {
            ++m_CurrentMagicByteIndex;

            //possible off by one error (solution is probably to move that ++ to below this if statement), but i thought pretty hard on it and think it's ok...
            if(m_CurrentMagicByteIndex == MAGIC_BYTE_SIZE)
            {
                //done with magic
                m_MagicGot = true;
                return true;
            }
            //else: need more magic (but LOOKS GOOD SO FAR), so: continue; -- this is implied since we'll get there anyways with how this while loop is set up..
        }
        else
        {
            //we got an unexpected byte, so we restart the magic seeking process. this doesn't necessarily mean we are out of bytes
            m_CurrentMagicByteIndex = 0;
        }
    }

    //we only get here if we read all the bytes available without ever seeing magic
    return false;
}
void NetworkMagic::messageHasBeenConsumedSoPlzResetMagic()
{
    m_MagicGot = false;
    m_CurrentMagicByteIndex = 0;
}
