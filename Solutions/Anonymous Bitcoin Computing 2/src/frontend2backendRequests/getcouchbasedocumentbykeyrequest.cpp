#include "getcouchbasedocumentbykeyrequest.h"

#include "Wt/WServer"
using namespace Wt;

#include "../frontend/anonymousbitcoincomputingwtgui.h"

//save constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput, CasMode casMode, GetAndSubscribeEnum subscribeMode)
    : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseGetKeyInput(couchbaseGetKeyInput), SaveCAS(casMode == DiscardCASMode ? false : true)
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
//load constructor
GetCouchbaseDocumentByKeyRequest::GetCouchbaseDocumentByKeyRequest()
    : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), SaveCAS(false), GetAndSubscribe(0)
{ }

void GetCouchbaseDocumentByKeyRequest::respond(std::string couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4), CouchbaseGetKeyInput, couchbaseDocument, lcbOpSuccess, dbError));
}
void GetCouchbaseDocumentByKeyRequest::respondWithCAS(std::string couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(GetAndSubscribe == 0)
    {
        Wt::WServer::instance()->post(WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished, AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4, _5), CouchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError));
        return;
    }

    //get and subscribe populate/update (TODOoptional: my use always wants CAS, but future proof means it shouldn't depend caller wanting it)
    Wt::WServer::instance()->post(WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE, AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3, _4, _5), CouchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError));
    //TODOoptional(was req, but finalize() should do it right when fallback would start being used, BUT fallback sounds more failsafe (session id changes or some such?)): maybe the fallback function passed to "Post" is our quickest way to determine a disconnect (unsubscribe), but really it might take just as long as the tradition disconnect (wapplication destroy), I am unsure. still makes sense to plug both points and to make sure that the backend can handle an unsubscribe request for a wapplication that isn't subscribed (race condition between the two)
}
