#include "ebookreaderbookmarksfileurlextractor.h"

#include <QIODevice>
#include <QFile>

#define TRY_TO_EXTRACT_BACKWARDS_UNTIL_PROTOCOL \
int indexofProtocol = qMax(bookmarksFile.lastIndexOf(http, indexOfEndOfUrlMark), bookmarksFile.lastIndexOf(https, indexOfEndOfUrlMark)); \
if(indexofProtocol > -1) \
{ \
    QByteArray url = bookmarksFile.mid(indexofProtocol, (indexOfEndOfUrlMark-indexofProtocol)); \
    QString urlStr(url); \
    emit urlExtracted(urlStr); \
}

//My eBook reader, which is "version 3" aka "keyboard" edition of well, probably the most popular brand of eBook readers on the planet (or at least 2nd best), which also runs linux as an aside, stores it's web browser's bookmarks in what `file` reports as Java Serialization Data. It's located at /var/local/java/prefs/browser/bookmarks_wv and staring at that bitch for a little bit using `xxd`, I noticed that all the URLs (except for the last one TO DOnereq) end with "xsq.~..t.". So I can start there and read backwards until I see an http:// or an https:// and then blamo extract the URL. It's worth noting that I'm coding this app ENTIRELY for porn. My eBook reader's bookmarks is 99% porn pics, and I want to view them in color some day (it will be like a weird form of nostalgia)
eBookReaderBookmarksFileUrlExtractor::eBookReaderBookmarksFileUrlExtractor(QObject *parent)
    : QObject(parent)
{ }
void eBookReaderBookmarksFileUrlExtractor::startExtractingUrlsFromEbookReaderBookmarksFile(QIODevice *bookmarksFileIoDevice)
{
    QByteArray bookmarksFile = bookmarksFileIoDevice->readAll();
    //QByteArray endOfUrlMark("xsq.~..t."); //nope. that's just how xxd visually represented it xxD
    const /*unsigned */char endOfUrlMarkRaw[] = { 0x78, 0x73, 0x71, 0x00, 0x7e, 0x00, 0x02, 0x74, 0x00 };
    //QByteArray endOfUrlMark = QByteArray::fromRawData(static_cast<const char*>(&endOfUrlMarkRaw), 9);
    QByteArray endOfUrlMark(endOfUrlMarkRaw);
    QByteArray http("http://");
    QByteArray https("https://");
    int indexOfEndOfUrlMark = 0;

    while((indexOfEndOfUrlMark = bookmarksFile.indexOf(endOfUrlMark, indexOfEndOfUrlMark)) > -1)
    {
        TRY_TO_EXTRACT_BACKWARDS_UNTIL_PROTOCOL
        ++indexOfEndOfUrlMark;
    }

    //now we COULD extract the very last url using 0x78, 0x78 (two lower case x's) as the endOfUrlMark, but if the url itself has xxx in it (which is quite likely), the extraction will fail. maybe I should just go "back 2 bytes from EOF then last index of to http[s]"? yea I guess I'll do that
    indexOfEndOfUrlMark = bookmarksFile.size()-2;
    TRY_TO_EXTRACT_BACKWARDS_UNTIL_PROTOCOL

    emit finishedExtractingUrlsFromEbookReaderBookmarksFile(true);
}
void eBookReaderBookmarksFileUrlExtractor::startExtractingUrlsFromEbookReaderBookmarksFile(const QString &bookmarksFilePath)
{
    QFile bookmarksFile(bookmarksFilePath);
    if(!bookmarksFile.open(QIODevice::ReadOnly))
    {
        emit finishedExtractingUrlsFromEbookReaderBookmarksFile(false);
        return;
    }
    startExtractingUrlsFromEbookReaderBookmarksFile(&bookmarksFile);
}
