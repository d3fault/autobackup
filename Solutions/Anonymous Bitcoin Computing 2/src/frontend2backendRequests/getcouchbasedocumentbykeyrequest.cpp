#include "getcouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "frontend/anonymousbitcoincomputingwtgui.h"

//save constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput, CasMode casMode)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseGetKeyInput(couchbaseGetKeyInput), SaveCAS(casMode == DiscardCASMode ? false : true)
{ }
//load constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), SaveCAS(false)
{ }

void GetCouchbaseDocumentByKeyRequest::respond(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes, bool lcbOpSuccess, bool dbError)
{
     Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes), lcbOpSuccess, dbError));
}
void GetCouchbaseDocumentByKeyRequest::respondWithCAS(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
     Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4, _5), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes), cas, lcbOpSuccess, dbError));
}
