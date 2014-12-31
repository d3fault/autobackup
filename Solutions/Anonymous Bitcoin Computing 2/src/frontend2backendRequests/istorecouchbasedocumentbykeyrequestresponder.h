#ifndef ISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
#define ISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H

#include <string>
#include <sys/types.h>

class IStoreCouchbaseDocumentByKeyRequestResponder
{
public:
    virtual void respond(bool lcbStoreModeIsAdd, const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseStoreKeyInput, bool lcbOpSuccess, bool dbError)=0;
    virtual void respondWithCas(const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseStoreKeyInput, u_int64_t casOutput, bool lcbOpSuccess, bool dbError)=0;
};

#endif // ISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
