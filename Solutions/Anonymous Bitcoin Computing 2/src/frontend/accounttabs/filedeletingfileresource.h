#ifndef FILEDELETINGFILERESOURCE_H
#define FILEDELETINGFILERESOURCE_H

#include <Wt/WFileResource>

using namespace Wt;
using namespace std;

class FileDeletingFileResource : public WFileResource
{
public:
    FileDeletingFileResource(const std::string& fileName, const string &suggestedFilename = "data.txt", const string &mimeType = "plain/text", DispositionType dispositionType = Attachment, WObject *parent = 0);
    ~FileDeletingFileResource();
};

#endif // FILEDELETINGFILERESOURCE_H
