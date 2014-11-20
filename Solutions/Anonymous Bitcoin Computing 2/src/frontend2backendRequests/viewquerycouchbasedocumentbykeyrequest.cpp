#include "viewquerycouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "../frontend/anonymousbitcoincomputingwtgui.h"

//should really be "query couchbase view by path request", but i cbf
ViewQueryCouchbaseDocumentByKeyRequest::ViewQueryCouchbaseDocumentByKeyRequest(const std::string &wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, const std::string &viewPath, int pageNum_MustBeGreaterThanOrEqualToOne)
    : WtSessionId(wtSessionId)
    , PointerToAnonymousBitcoinComputingWtGUI(pointerToAnonymousBitcoinComputingWtGUI)
    , ViewPath(viewPath)
    , PageNum_WithOneBeingTheFirstPage(pageNum_MustBeGreaterThanOrEqualToOne)
{ }
//TODOoptimization: get/store have respond as static, idk why. unnecessary derefs galore
void ViewQueryCouchbaseDocumentByKeyRequest::respond(const ViewQueryPageContentsType &pageContents, int totalPages, bool internalServerErrorOrJsonError)
{
    Wt::WServer::instance()->post(WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::queryCouchbaseViewFinished, PointerToAnonymousBitcoinComputingWtGUI, _1, _2, _3), pageContents, totalPages, internalServerErrorOrJsonError));
}
