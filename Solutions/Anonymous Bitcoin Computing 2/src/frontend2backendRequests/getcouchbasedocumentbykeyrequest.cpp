#include "getcouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "../frontend/anonymousbitcoincomputingwtgui.h"

//save constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput, CasMode casMode, GetAndSubscribeEnum subscribeMode)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseGetKeyInput(couchbaseGetKeyInput), SaveCAS(casMode == DiscardCASMode ? false : true), GetAndSubscribe(subscribeMode == GetAndSubscribeMode ? true : false)
{ }
//load constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), SaveCAS(false), GetAndSubscribe(false)
{ }

void GetCouchbaseDocumentByKeyRequest::respond(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4), originalRequest->CouchbaseGetKeyInput, std::string(static_cast<const char*>(couchbaseDocument), couchbaseDocumentSizeBytes), lcbOpSuccess, dbError));
}
void GetCouchbaseDocumentByKeyRequest::respondWithCAS(GetCouchbaseDocumentByKeyRequest *originalRequest, std::string couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(!originalRequest->GetAndSubscribe)
    {
        Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4, _5), originalRequest->CouchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError));
        return;
    }

    //get and subscribe populate/update
    Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4, _5), originalRequest->CouchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError));
}
