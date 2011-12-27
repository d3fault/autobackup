#include "curldownloader.h"

void CurlDownloader::init()
{
    m_Curl = curl_easy_init();
    if(m_Curl)
    {
        freshCurl();
    }
    else
    {
        emit d("curl init failed");
        curlError();
    }
}
void CurlDownloader::load()
{
    curl_easy_setopt(m_Curl, CURLOPT_URL, "http://127.0.0.1:6969/cont.avi");

    m_CurlResult = curl_easy_perform(m_Curl);
    if(m_CurlResult == CURLE_OK)
    {
        emit d("curl: done downloading file");
    }
    else
    {
        curlError();
    }
}
void CurlDownloader::freshCurl()
{
    if(m_Curl)
    {
        curl_easy_cleanup(m_Curl);
        m_Curl = curl_easy_init();

        //ssl
        //curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYPEER, 1);
        //curl_easy_setopt(m_Curl, CURLOPT_CAINFO, "ca-bundle.crt");

        //user-agent
        curl_easy_setopt(m_Curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; rv:1.9) Gecko/20100101 Firefox/4.0");

        //data
        curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, &CurlDownloader::staticWriteFunction);
    }
}
void CurlDownloader::curlError()
{
    emit d("Curl Error: " + QString::number(m_CurlResult));
}
size_t CurlDownloader::staticWriteFunction(void *ptr, size_t size, size_t nmemb, void *classPointer)
{
    return static_cast<CurlDownloader*>(classPointer)->writeFunction(ptr, size, nmemb);
}
size_t CurlDownloader::writeFunction(void *ptr, size_t size, size_t nmemb)
{
    QByteArray *newData = new QByteArray((const char*)ptr);
    emit onDataGathered(*newData);
    return size * nmemb;
}
