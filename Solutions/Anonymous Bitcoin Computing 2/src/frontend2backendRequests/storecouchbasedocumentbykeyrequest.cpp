#include "storecouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "istorecouchbasedocumentbykeyrequestresponder.h"

//save constructor no cas overload
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest(IStoreCouchbaseDocumentByKeyRequestResponder *responder, std::string wtSessionId, void *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseStoreKeyInput, std::string couchbaseStoreDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode, WhatToDoWithOutputCasEnum whatToDoWithOutputCas)
    : WtSessionId(wtSessionId)
    , AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI)
    , CouchbaseStoreKeyInput(couchbaseStoreKeyInput)
    , CouchbaseStoreDocumentInput(couchbaseStoreDocumentInput)
    , LcbStoreModeIsAdd(lcbStoreMode == AddMode ? true : false)
    , HasCasInput(lcbStoreMode == SetWithCasMode ? true : false)
    , SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
    , m_Responder(responder)
{ }
//save constructor cas overload
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest(IStoreCouchbaseDocumentByKeyRequestResponder *responder, std::string wtSessionId, void *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseStoreKeyInput, std::string couchbaseStoreDocumentInput, u_int64_t cas, WhatToDoWithOutputCasEnum whatToDoWithOutputCas)
    : WtSessionId(wtSessionId)
    , AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI)
    , CouchbaseStoreKeyInput(couchbaseStoreKeyInput)
    , CouchbaseStoreDocumentInput(couchbaseStoreDocumentInput)
    , LcbStoreModeIsAdd(false)
    , HasCasInput(true)
    , CasInput(cas)
    , SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
    , m_Responder(responder)
{ }
#if 0
//load constructor
StoreCouchbaseDocumentByKeyRequest::StoreCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), LcbStoreModeIsAdd(true), HasCasInput(false), SaveCasOutput(false)
{ }
#endif
void StoreCouchbaseDocumentByKeyRequest::respond(bool lcbOpSuccess, bool dbError)
{
    m_Responder->respond(LcbStoreModeIsAdd, WtSessionId, AnonymousBitcoinComputingWtGUIPointerForCallback, CouchbaseStoreKeyInput, lcbOpSuccess, dbError);
}
void StoreCouchbaseDocumentByKeyRequest::respondWithCas(u_int64_t casOutput, bool lcbOpSuccess, bool dbError)
{
    m_Responder->respondWithCas(WtSessionId, AnonymousBitcoinComputingWtGUIPointerForCallback, CouchbaseStoreKeyInput, casOutput, lcbOpSuccess, dbError);
}
