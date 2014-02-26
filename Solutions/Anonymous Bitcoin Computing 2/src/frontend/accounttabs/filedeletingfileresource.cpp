#include "filedeletingfileresource.h"

#include <cstdio>

//Simple wrapper around WFileResources. Takes ownership of the file on the filesystem and deletes it at destruction
FileDeletingFileResource::FileDeletingFileResource(const std::string& fileName, const std::string &suggestedFilename, const std::string& mimeType, DispositionType dispositionType, WObject *parent)
    : WFileResource(mimeType, fileName, parent)
{
    suggestFileName(suggestedFilename, dispositionType);
}
FileDeletingFileResource::~FileDeletingFileResource()
{
    beingDeleted();

    if(remove(this->fileName().c_str()) != 0)
    {
        cerr << "Failed to remove: " << this->fileName() << " in ~FileDeletingFileResource()" << endl;
    }
}
//^from filesystem might be slower (probably not since tmpfs is most likely going to be used), but using WFileResource will likely be much faster and memory efficient (err wait just contradicted myself there (unless i DON'T use tmpfs))
