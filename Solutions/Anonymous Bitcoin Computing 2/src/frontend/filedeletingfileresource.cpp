#include "filedeletingfileresource.h"

#include <boost/algorithm/string/predicate.hpp>
#include <cstdio>

//Simple wrapper around WFileResources. Takes ownership of the file on the filesystem and deletes it at destruction
FileDeletingFileResource::FileDeletingFileResource(const std::string& mimeType, const std::string& fileName, const std::string &suggestedExtension, WObject *parent)
    : WFileResource(mimeType, fileName, parent)//, m_AdImageBytes(adImageBytes), m_MimeType(extensionAndMimeType.second)
{
    suggestFileName("image." + suggestedExtension, Inline);
    setDispositionType(Inline);
}
FileDeletingFileResource::~FileDeletingFileResource()
{
    beingDeleted();

    if(remove(this->fileName().c_str()) != 0)
    {
        cerr << "Failed to remove: " << this->fileName() << " in ~FileDeletingFileResource()" << endl;
    }
}
#if 0
void AdImagePreviewResource::handleRequest(const Http::Request &request, Http::Response &response)
{
    response.setMimeType("image/" + m_MimeType); //TODOreq: use the uploaded filename [safely], not the spool filename, to safely guess which mime type it is. Fallback to jpeg. image/gif, image/png, image/bmp, image/svg+xml (TODOreq: svg/xml might be security concern if scripts are allowed by default (fucking browsers)). I really with there was just image/* :-P

    response.out() << m_AdImageBytes; //TODOreq: wouldn't a null byte fuck shit up? Really though, it might only just end it early so might not matter security-wise.
    //TODOreq: the example had an endl after it (but it was also text). HTTP is pretty stupid, so that endl might be necessary xD

    //TODOoptimization: response continuation might be optimial, but I'm having trouble determining a chunk size... and also unsure of what it's implications are. Is a continuation a round trip to the client, or just an internal mechanism for whenever more bytes are needed?

    //TODOoptimization: since I know in this case the image will only be requested once, I could delete/deallocate the bytes here since I just streamed them. It's not only an optimization, but might be the best way to do it object oriented like (HOWEVER I think we're on a different thread, idk if that matters (if it doesn't then I should have sent couchbase a fucking const char * to the b64 image)). And I should change the class name from AdImagePreviewResource to SingleUseAdImagePreviewResource
}
#endif
//^from filesystem might be slower (probably not since tmpfs is most likely going to be used), but using WFileResource will likely be much faster and memory efficient (err wait just contradicted myself there (unless i DON'T use tmpfs))

//pair because "svg+xml" wouldn't make a good filename extension (i'm completely surrounded by morons)...
pair<string, string> FileDeletingFileResource::guessExtensionAndMimeType(string filename)
{
    //std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower); //ugly, Qt5Life (had: <guess>)

    //boost > std/stl, but boost < qt. already depending on boost though, so...
    boost::algorithm::to_lower(filename);

    pair<string,string> guessedExtensionAndMime("jpeg", "jpeg");
    if(boost::algorithm::ends_with(filename, ".jpg") || boost::algorithm::ends_with(filename, ".jpeg"))
    {
        return guessedExtensionAndMime;
    }
    if(boost::algorithm::ends_with(filename, ".png"))
    {
        guessedExtensionAndMime.first = "png";
        guessedExtensionAndMime.second = "png";
        return guessedExtensionAndMime;
    }
    if(boost::algorithm::ends_with(filename, ".gif"))
    {
        guessedExtensionAndMime.first = "gif";
        guessedExtensionAndMime.second = "gif";
        return guessedExtensionAndMime;
    }
    if(boost::algorithm::ends_with(filename, ".bmp")) //TODOoptimization: maybe security risk, but would save bandwidth, to convert this to png
    {
        guessedExtensionAndMime.first = "bmp";
        guessedExtensionAndMime.second = "bmp";
        return guessedExtensionAndMime;
    }
    if(boost::algorithm::ends_with(filename, ".svg"))
    {
        guessedExtensionAndMime.first = "svg";
        guessedExtensionAndMime.second = "svg+xml";
        return guessedExtensionAndMime;
    }
    if(boost::algorithm::ends_with(filename, ".svg"))
    {
        guessedExtensionAndMime.first = "svg";
        guessedExtensionAndMime.second = "svg+xml";
        return guessedExtensionAndMime;
    }
    if(boost::algorithm::ends_with(filename, ".webp"))
    {
        guessedExtensionAndMime.first = "webp";
        guessedExtensionAndMime.second = "webp";
        return guessedExtensionAndMime;
    }
    return guessedExtensionAndMime;
}
