#ifndef FILEDELETINGFILERESOURCE_H
#define FILEDELETINGFILERESOURCE_H

#include <Wt/WFileResource>

#include <boost/algorithm/string.hpp>

using namespace Wt;
using namespace std;

class FileDeletingFileResource : public WFileResource
{
public:
    FileDeletingFileResource(const std::string& mimeType, const std::string& fileName, const std::string &suggestedExtension, WObject *parent = 0);
    ~FileDeletingFileResource();
    static pair<string,string> guessExtensionAndMimeType(string filename);
};

#endif // FILEDELETINGFILERESOURCE_H
