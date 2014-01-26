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

void GetCouchbaseDocumentByKeyRequest::respond(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes)
{
     Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes)));
}
void GetCouchbaseDocumentByKeyRequest::respondWithCAS(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes, u_int64_t cas)
{
     Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes), cas));
}
