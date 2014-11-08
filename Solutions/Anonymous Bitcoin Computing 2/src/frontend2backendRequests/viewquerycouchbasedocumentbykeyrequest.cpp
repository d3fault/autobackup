#include "viewquerycouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "../frontend/anonymousbitcoincomputingwtgui.h"

ViewQueryCouchbaseDocumentByKeyRequest::ViewQueryCouchbaseDocumentByKeyRequest(const std::string &wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, const std::string &viewPath, int pageNum_MustBeGreaterThanOrEqualToOne)
    : WtSessionId(wtSessionId)
    , PointerToAnonymousBitcoinComputingWtGUI(pointerToAnonymousBitcoinComputingWtGUI)
    , ViewPath(viewPath)
    , PageNum_WithOneBeingTheFirstPage(pageNum_MustBeGreaterThanOrEqualToOne)
{ }
//TODOoptimization: get/store have respond as static, idk why. unnecessary derefs galore
void ViewQueryCouchbaseDocumentByKeyRequest::respond(const ViewQueryPageContentsType &pageContents, bool internalServerErrorOrJsonError)
{
    //our thread safety mechanism for now is to make a COPY of the list for the response
    ViewQueryPageContentsType *pageContentsCopy = new ViewQueryPageContentsType(pageContents); //TODOreq: frontend takes ownership and deletes after using, until shared_ptr strat is implemented
    Wt::WServer::instance()->post(WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::queryCouchbaseViewFinished, PointerToAnonymousBitcoinComputingWtGUI, _1, _2), pageContentsCopy, internalServerErrorOrJsonError));
}
