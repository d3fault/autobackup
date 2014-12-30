#include "abcguigetcouchbasedocumentbykeyrequestresponder.h"

#include <Wt/WServer>

#include "../frontend/anonymousbitcoincomputingwtgui.h"

void AbcGuiGetCouchbaseDocumentByKeyRequestResponder::respond(const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, static_cast<AnonymousBitcoinComputingWtGUI*>(anonymousBitcoinComputingWtGUIPointerForCallback), _1, _2, _3, _4), couchbaseGetKeyInput, couchbaseDocument, lcbOpSuccess, dbError));
}
void AbcGuiGetCouchbaseDocumentByKeyRequestResponder::respondWithCAS(unsigned char getAndSubscribe, const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(getAndSubscribe == 0)
    {
        Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished, static_cast<AnonymousBitcoinComputingWtGUI*>(anonymousBitcoinComputingWtGUIPointerForCallback), _1, _2, _3, _4, _5), couchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError));
        return;
    }

    //get and subscribe populate/update (TODOoptional: my use always wants CAS, but future proof means it shouldn't depend caller wanting it)
    Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE, static_cast<AnonymousBitcoinComputingWtGUI*>(anonymousBitcoinComputingWtGUIPointerForCallback), _1, _2, _3, _4, _5), couchbaseGetKeyInput, couchbaseDocument, cas, lcbOpSuccess, dbError));
    //TODOoptional(was req, but finalize() should do it right when fallback would start being used, BUT fallback sounds more failsafe (session id changes or some such?)): maybe the fallback function passed to "Post" is our quickest way to determine a disconnect (unsubscribe), but really it might take just as long as the tradition disconnect (wapplication destroy), I am unsure. still makes sense to plug both points and to make sure that the backend can handle an unsubscribe request for a wapplication that isn't subscribed (race condition between the two)
}
