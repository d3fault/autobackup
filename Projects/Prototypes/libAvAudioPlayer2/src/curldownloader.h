#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <iloadmediastreamsintomemory.h>

class CurlDownloader : public ILoadMediaStreamsIntoMemory
{
public slots:
    void init();
    void loadLoop();
};

#endif // CURLDOWNLOADER_H
