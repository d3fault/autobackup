#include "storecouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "frontend/anonymousbitcoincomputingwtgui.h"

//save constructor no cas overload
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode, WhatToDoWithOutputCasEnum whatToDoWithOutputCas)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput), LcbStoreModeIsAdd(lcbStoreMode == AddMode ? true : false), HasCasInput(lcbStoreMode == StoreWithCasMode ? true : false), SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
{ }
//save constructor cas overload
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, u_int64_t cas, WhatToDoWithOutputCasEnum whatToDoWithOutputCas)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput), LcbStoreModeIsAdd(false), HasCasInput(true), CasInput(cas), SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
{ }
//load constructor
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), LcbStoreModeIsAdd(true), HasCasInput(false), SaveCasOutput(false)
{ }

void StoreCouchbaseDocumentByKeyRequest::respond(StoreCouchbaseDocumentByKeyRequest *originalRequest, bool lcbOpSuccess, bool dbError)
{
    if(!originalRequest->LcbStoreModeIsAdd)
    {
        Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3), originalRequest->CouchbaseAddKeyInput, lcbOpSuccess, dbError));
        return;
    }
    Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeWIthoutInputCasCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3), originalRequest->CouchbaseAddKeyInput, lcbOpSuccess, dbError));
}
void StoreCouchbaseDocumentByKeyRequest::respondWithCas(StoreCouchbaseDocumentByKeyRequest *originalRequest, u_int64_t casOutput, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4), originalRequest->CouchbaseAddKeyInput, casOutput, lcbOpSuccess, dbError));
}
