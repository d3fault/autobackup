#ifndef GETCOUCHBASEDOCUMENTBYKEYREQUEST_H
#define GETCOUCHBASEDOCUMENTBYKEYREQUEST_H

#include "Wt/WServer"
using namespace Wt;

#include "frontend/anonymousbitcoincomputingwtgui.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

class GetCouchbaseDocumentByKeyRequest
{
public:
    enum CasMode
    {
        SaveCASMode,
        DiscardCASMode
    };
    //save constructor
    GetCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput, CasMode casMode = DiscardCASMode)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseGetKeyInput(couchbaseGetKeyInput), SaveCAS(casMode == DiscardCASMode ? false : true)
    { }
    //load constructor
    GetCouchbaseDocumentByKeyRequest()
        : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), SaveCAS(false)
    { }

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseGetKeyInput;
    bool SaveCAS;

    static inline void respond(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes)
    {
         Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes)));
    }
    static inline void respondWithCAS(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes, u_int64_t cas)
    {
         Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2, _3), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes), cas));
    }
private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive &ar, const unsigned int version) const
    {
        (void)version;
        ar & WtSessionId;

        char AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI];
        memcpy(&AddressToAnonymousBitcoinComputingWtGUIAsString, &AnonymousBitcoinComputingWtGUIPointerForCallback, LengthOfAddressToAnonymousBitcoinComputingWtGUI);
        AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI-1] = '\0';
        ar & boost::serialization::make_binary_object(&AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI);

        ar & CouchbaseGetKeyInput;
        ar & SaveCAS;
    }
    template<class Archive>
    void load(Archive &ar, const unsigned int version)
    {
        (void)version;
        ar & WtSessionId;

        char AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI];
        ar & boost::serialization::make_binary_object(&AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI);
        memcpy(&AnonymousBitcoinComputingWtGUIPointerForCallback, &AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI-1);

        ar & CouchbaseGetKeyInput;
        ar & SaveCAS;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // GETCOUCHBASEDOCUMENTBYKEYREQUEST_H

