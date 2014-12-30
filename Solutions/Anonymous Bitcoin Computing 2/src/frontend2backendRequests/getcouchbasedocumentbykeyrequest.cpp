#include "getcouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "igetcouchbasedocumentbykeyrequestresponder.h"
#include "../frontend/anonymousbitcoincomputingwtgui.h"

//save constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest(IGetCouchbaseDocumentByKeyRequestResponder *responder, std::string wtSessionId, void *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput, CasMode casMode, GetAndSubscribeEnum subscribeMode)
    : WtSessionId(wtSessionId)
    , AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI)
    , CouchbaseGetKeyInput(couchbaseGetKeyInput)
    , SaveCAS(casMode == DiscardCASMode ? false : true)
    , m_Responder(responder)
{
    if(subscribeMode == JustGetDontSubscribeMode)
    {
        GetAndSubscribe = 0; //if changing value from zero, change in load constructor as well
    }
    else if(subscribeMode == GetAndSubscribeMode)
    {
        GetAndSubscribe = 1;
    }
    else if(subscribeMode == GetAndSubscribeChangeSessionIdMode)
    {
        GetAndSubscribe = 2;
    }
    else if(subscribeMode == GetAndSubscribeUnsubscribeMode)
    {
        GetAndSubscribe = 3;
    }
    else if(subscribeMode == GetAndSubscribeJustKiddingNoJavascriptHereSoIjustWantONEsubscriptionUpdateValOrAHardGetIfNeedBeKthx)
    {
        GetAndSubscribe = 4; //TODOoptional: it's probably fine to just serialize the enum itself, but eh fuggit
    }
    else if(subscribeMode == GetAndSubscribeJustKiddingNoJavascriptHereSoIjustWantONEsubscriptionUpdateValOrAHardGetIfNeedBeKthxDONTUSETHIS5ISRESERVEDASAFUCKINGCRAZYASSHACK)
    {
        GetAndSubscribe = 5; //'reserved' number used in hack, don't ask. look for the number '5' in the backend to see how this hack is used. basically we want the next+1 get and subscribe update...
    }
    else if(subscribeMode == GetAndSubscribeJustKiddingNoJavascriptHereSoIjustWantONEsubscriptionUpdateValOrAHardGetIfNeedBeKthxEVENMOREHACKSFORTHEHARDGETTHING)
    {
        GetAndSubscribe = 6; //another hack that i'm just 'reserving', but this time for when the no-js thingo does a hard db get...
    }
}
#if 0
//load constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), SaveCAS(false), GetAndSubscribe(0)
{ }
#endif
void GetCouchbaseDocumentByKeyRequest::respond(std::string couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    m_Responder->respond(WtSessionId, AnonymousBitcoinComputingWtGUIPointerForCallback, CouchbaseGetKeyInput, couchbaseDocument, lcbOpSuccess, dbError);
}
void GetCouchbaseDocumentByKeyRequest::respondWithCAS(std::string couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    m_Responder->respondWithCAS(GetAndSubscribe, WtSessionId, AnonymousBitcoinComputingWtGUIPointerForCallback, CouchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError);
}
