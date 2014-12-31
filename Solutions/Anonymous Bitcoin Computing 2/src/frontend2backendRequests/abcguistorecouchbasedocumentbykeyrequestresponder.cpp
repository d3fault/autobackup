#include "abcguistorecouchbasedocumentbykeyrequestresponder.h"

#include <Wt/WServer>

#include "../frontend/anonymousbitcoincomputingwtgui.h"

void AbcGuiStoreCouchbaseDocumentByKeyRequestResponder::respond(bool lcbStoreModeIsAdd, const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseStoreKeyInput, bool lcbOpSuccess, bool dbError)
{
    if(!lcbStoreModeIsAdd)
    {
        Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyWithInputCasFinished, static_cast<AnonymousBitcoinComputingWtGUI*>(anonymousBitcoinComputingWtGUIPointerForCallback), _1, _2, _3), couchbaseStoreKeyInput, lcbOpSuccess, dbError));
        return;
    }
    Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeWithoutInputCasCouchbaseDocumentByKeyFinished, static_cast<AnonymousBitcoinComputingWtGUI*>(anonymousBitcoinComputingWtGUIPointerForCallback), _1, _2, _3), couchbaseStoreKeyInput, lcbOpSuccess, dbError));
}
void AbcGuiStoreCouchbaseDocumentByKeyRequestResponder::respondWithCas(const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseStoreKeyInput, u_int64_t casOutput, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished, static_cast<AnonymousBitcoinComputingWtGUI*>(anonymousBitcoinComputingWtGUIPointerForCallback), _1, _2, _3, _4), couchbaseStoreKeyInput, casOutput, lcbOpSuccess, dbError));
}
