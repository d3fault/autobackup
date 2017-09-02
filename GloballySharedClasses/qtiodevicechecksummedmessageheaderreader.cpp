#include "qtiodevicechecksummedmessageheaderreader.h"

#include <QIODevice>

#include "qtiodevicechecksummedmessagereader.h"

#define ChecksummedMessageHeaderReader_SYNC "SYNC"

QtIoDeviceChecksummedMessageHeaderReader::QtIoDeviceChecksummedMessageHeaderReader(QIODevice *ioDevice)
    : m_IoDevice(ioDevice)
    , m_State(LookingForSync)
    , SizeOfLargestHeaderComponent(32 /*32 hex digits for MD5 Checksums*/)
{ }
bool QtIoDeviceChecksummedMessageHeaderReader::tryReadUntilComma()
{
    //TO DOnereq: the json inputCommand must not contain the word "SYNC". it can and will contain assloads of commas, which yes if we do get out of sync will trigger this shiz yada yada, but it's fine as long as SYNC isn't seen. actually fuck it, come to think of it even that doesn't matter... because the fkn checksums gotta check out ;-P. my protocol is finally robust
    while(m_IoDevice->bytesAvailable())
    {
        char inChar;
        if(!m_IoDevice->getChar(&inChar)) //TODOoptimization: optimize this.... LATER
        {
            m_State = LookingForSync;
            return false;
        }
        if(inChar == ',') //TODOreq: shared delim def in header
            return true;
        m_MessageHeaderBuffer.append(inChar);

        if(m_MessageHeaderBuffer.length() > SizeOfLargestHeaderComponent)
        {
            //while reading here and filling up m_MessageHeaderBuffer, we need to also occassionally shrink it. if we got out of sync then it might grow quite large before we see a comma again. the max size it needs to be is the biggest size of the 3 different message headers, so I'm guessing the hex encoding of [either of] the checksum[s] is going to be the biggest and therefore the max size we should allow m_MessageHeaderBuffer to become. we should drop characters starting from the very left, since new ones are appended on the right
            m_MessageHeaderBuffer = m_MessageHeaderBuffer.mid(1);
        }
    }
    return false;
}
bool QtIoDeviceChecksummedMessageHeaderReader::tryReadMessageHeader(int *out_MessageSize)
{
    switch(m_State)
    {
        case LookingForSync:
        {
            if(!tryReadUntilComma())
            {
                return false;
                break;
            }
            if(m_MessageHeaderBuffer.endsWith(ChecksummedMessageHeaderReader_SYNC))
            {
                m_State = ReadingChecksumOfSizeOfData;
            }
            m_MessageHeaderBuffer.clear();
        }
        //INTENTIONALLY NOT BREAKING HERE
        case ReadingChecksumOfSizeOfData:
        {
            if(!tryReadUntilComma())
            {
                return false;
                break;
            }
            m_ParsedChecksumOfSizeOfData = m_MessageHeaderBuffer;
            m_MessageHeaderBuffer.clear();
            m_State = ReadingSizeOfData;
        }
        //INTENTIONALLY NOT BREAKING HERE
        case ReadingSizeOfData:
        {
            if(!tryReadUntilComma())
            {
                return false;
                break;
            }
            bool convertOk;
            m_ParsedSizeOfData = m_MessageHeaderBuffer.toInt(&convertOk);
            if(!convertOk)
            {
                m_State = LookingForSync;
                return false;
            }
            m_ParsedSizeOfDataAsString = m_MessageHeaderBuffer;
            m_MessageHeaderBuffer.clear();
            m_CalculatedChecksumOfSizeOfData = QtIoDeviceChecksummedMessageReader::checksum(m_ParsedSizeOfDataAsString);
            if(m_CalculatedChecksumOfSizeOfData != m_ParsedChecksumOfSizeOfData)
            {
                //qFatal("m_CalculatedChecksumOfSizeOfData != m_ParsedChecksumOfSizeOfData"); //TODOreq: "request message is re-sent"
                m_State = LookingForSync;
                return false;
                break;
            }
            m_State = ReadingChecksumOfData;
        }
        //INTENTIONALLY NOT BREAKING HERE
        case ReadingChecksumOfData:
        {
            if(!tryReadUntilComma())
            {
                return false;
                break;
            }
            m_ParsedMessageChecksum = m_MessageHeaderBuffer; //we have to wait until we read in the data before using this checksum
            m_MessageHeaderBuffer.clear();
            *out_MessageSize = m_ParsedSizeOfData;
            m_State = LookingForSync;
            return true;
        }
        break;
    }
    return false; //should (will) never get here
}
const QByteArray &QtIoDeviceChecksummedMessageHeaderReader::parsedMessageChecksum() const
{
    return m_ParsedMessageChecksum;
}
