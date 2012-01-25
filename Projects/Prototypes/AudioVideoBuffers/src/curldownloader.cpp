#include "curldownloader.h"

CurlDownloader::CurlDownloader(LocalFile *localFile)
{
    m_LocalFile = localFile;
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
    //i'm considering making the call to giveMeADataBuffer() async, but at the same time i don't want to block curl and let it's buffers overflow (if they would)
#if 0
    GeneratedDataBuffer *m_EmptyDataBufferToWriteDownloadedDataInto = m_DataBufferGeneator->giveMeADataBuffer();
#endif



    //i guess effectively i need to give curl infinite (until unable to allocate more) memory.. assuming the memory isn't written to hard drive and re-used fast enough, which i'm pretty sure it will be. networkSpeed < hddSpeed. still, i feel the need to give it a cap. what will this do? it will say basically that if my hdd can't keep up with the network/memory, this is the max amount i will have in memory at a given time. this will cause curl or kernel network buffers to overflow and data to be lossed. the cap is a bad idea in this context. still, something like 50mb seems like plenty. even 10mb, the hdd should very easily be able to keep up. modern network speeds are like 2mb/s where i live. hdd speeds are like 50mb/s so eh i don't think the hdd will ever be a bottleneck. so i SHOULD enforce a cache size.
    //agg, the hdd IS the cache. the memory is just another level of the cache. how big do i want my memory cache to be? anything
    //this is all LocalFile logic. does it need it's own buffer system?

    //Curl -> LocalFile[10mb in-memory cache, currentPosition+10mb, 100% to hdd] <--> Decoder <--> AudioPlayer
    //<--> indicates pull from or push to. decoder pulls from LocalFile, but is initially pushed to by it. AudioPlayer pulls from Decoder, but is initially pushed to by it

    //i don't think curl is going to use this DataBufferGenerator. It's only knowledge should be of LocalFile, which can maybe use it but maybe it shouldn't? since I think we need 1 and at most 2 buffers. 1 for the buffer we want to keep, and the 2nd one just for shit we haven't yet written to disk. the disk lag cache

    //we should just pass the data pointer to localfile as well as the size and let LocalFile do with it whatever it needs
#if 0
    emit dataGathered(new QByteArray(dataPtr)); //actually, this is probably not a good idea because the dataPtr is likely to change by the time LocalFile gets it. Curl is on it's own thread, LocalFile is on it's own thread. maybe i DO use DataBufferGenerator?
#endif

    //curl presents us a const char* and a dataSize, we use our own thread to pass it along to localFile and queue it for use on it's own thread for saving/reading later
    m_LocalFile->append(dataPtr, dataPtrSize); //localfile is on a different thread, but append uses mutexes to protect whatever it needs to
}
