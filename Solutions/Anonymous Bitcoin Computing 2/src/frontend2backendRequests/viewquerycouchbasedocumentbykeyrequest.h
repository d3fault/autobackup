#ifndef VIEWQUERYCOUCHBASEDOCUMENTBYKEYREQUEST_H
#define VIEWQUERYCOUCHBASEDOCUMENTBYKEYREQUEST_H

#include "../abc2common.h"

class AnonymousBitcoinComputingWtGUI;

class ViewQueryCouchbaseDocumentByKeyRequest
{
public:
    ViewQueryCouchbaseDocumentByKeyRequest(const std::string &wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, const std::string &viewPath, int pageNum_MustBeGreaterThanOrEqualToOne);

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *PointerToAnonymousBitcoinComputingWtGUI;
    std::string ViewPath;
    int PageNum_WithOneBeingTheFirstPage;

    void respond(const ViewQueryPageContentsType &pageContents, bool internalServerErrorOrJsonError = false);
};

#endif // VIEWQUERYCOUCHBASEDOCUMENTBYKEYREQUEST_H
