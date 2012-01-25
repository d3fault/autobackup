#include "curldownloader.h"

CurlDownloader::CurlDownloader(DataBufferGenerator *dataBufferGenerator)
{
    m_DataBufferGeneator = dataBufferGenerator;
}
void CurlDownloader::start()
{
    //normally this is where we'd start the downloading of the file. we're already on our own thread, so it doesn't matter that curl blocks. actually it might if we try to pause/stop the download. we can't just send the curl thread an event, it would never process it :(. pretty sure there is a curl async though... for now just use the sync because you know it works. we just won't stop/pause yet. ANYWAYS, we're just going to use a timer to simulate the downloading/copying of data
    //in OUR test right now, we CAN process events in between data signals. i bet the multi interface is easy enough.
    if(!m_Timer)
    {
        m_Timer = new QTimer(this);
        connect(m_Timer, SIGNAL(timeout()), this, SLOT(simulateReceivingData()));
    }
    m_Timer->start(10);
}
void CurlDownloader::simulateReceivingData()
{
    GeneratedDataBuffer *m_EmptyDataBufferToWriteDownloadedDataInto = m_DataBufferGeneator->giveMeADataBuffer();
}
