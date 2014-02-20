#include "storecouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "../frontend/anonymousbitcoincomputingwtgui.h"

//save constructor no cas overload
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseStoreKeyInput, std::string couchbaseStoreDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode, WhatToDoWithOutputCasEnum whatToDoWithOutputCas)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseStoreKeyInput(couchbaseStoreKeyInput), CouchbaseStoreDocumentInput(couchbaseStoreDocumentInput), LcbStoreModeIsAdd(lcbStoreMode == AddMode ? true : false), HasCasInput(lcbStoreMode == SetWithCasMode ? true : false), SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
{ }
//save constructor cas overload
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseStoreKeyInput, std::string couchbaseStoreDocumentInput, u_int64_t cas, WhatToDoWithOutputCasEnum whatToDoWithOutputCas)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseStoreKeyInput(couchbaseStoreKeyInput), CouchbaseStoreDocumentInput(couchbaseStoreDocumentInput), LcbStoreModeIsAdd(false), HasCasInput(true), CasInput(cas), SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
{ }
//load constructor
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), LcbStoreModeIsAdd(true), HasCasInput(false), SaveCasOutput(false)
{ }

void StoreCouchbaseDocumentByKeyRequest::respond(StoreCouchbaseDocumentByKeyRequest *originalRequest, bool lcbOpSuccess, bool dbError)
{
    if(!originalRequest->LcbStoreModeIsAdd)
    {
        Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyWithInputCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3), originalRequest->CouchbaseStoreKeyInput, lcbOpSuccess, dbError));
        return;
    }
    Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeWithoutInputCasCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3), originalRequest->CouchbaseStoreKeyInput, lcbOpSuccess, dbError));
}
void StoreCouchbaseDocumentByKeyRequest::respondWithCas(StoreCouchbaseDocumentByKeyRequest *originalRequest, u_int64_t casOutput, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4), originalRequest->CouchbaseStoreKeyInput, casOutput, lcbOpSuccess, dbError));
}
