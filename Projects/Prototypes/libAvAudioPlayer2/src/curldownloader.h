#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <iloadmediastreamsintomemory.h>

#include <curl/curl.h>

class CurlDownloader : public ILoadMediaStreamsIntoMemory
{
private:
    CURL *m_Curl;
    CURLcode m_CurlResult;
    static size_t staticWriteFunction(void *ptr, size_t size, size_t nmemb, void *classPointer);
    size_t writeFunction(void *ptr, size_t size, size_t nmemb);

    void freshCurl();
    void curlError();
public slots:
    void init();
    void load();
};

#endif // CURLDOWNLOADER_H
