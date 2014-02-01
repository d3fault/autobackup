#ifndef GETCOUCHBASEDOCUMENTBYKEYREQUEST_H
#define GETCOUCHBASEDOCUMENTBYKEYREQUEST_H

#include <cstring>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

class AnonymousBitcoinComputingWtGUI;

class GetCouchbaseDocumentByKeyRequest
{
public:
    enum CasMode
    {
        SaveCASMode,
        DiscardCASMode
    };
    enum GetAndSubscribeEnum
    {
        JustGetDontSubscribeMode,
        GetAndSubscribeMode,

        //in both of these modes, the backend does not respond and the frontend always assumes it works (TODOoptimization: expect response in frontend using timeout or some such. blah probably a dumb idea because if the backend doesn't do it, shit is ALREADY fucked)
        GetAndSubscribeChangeSessionIdMode,
        GetAndSubscribeUnsubscribeMode
        //TODOreq: maybe this is where the js special 'get-FROM-subscription-but-dont-subscribe-unless-there-isnt-one-in-which-case-do-a-full-get-fuck-it-if-its-costly-ish' (tm)
    };
    //save constructor
    GetCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput, CasMode casMode = DiscardCASMode, GetAndSubscribeEnum subscribeMode = JustGetDontSubscribeMode);
    //load constructor
    GetCouchbaseDocumentByKeyRequest();

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseGetKeyInput;
    bool SaveCAS;
    unsigned char GetAndSubscribe;

    static void respond(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes, bool lcbOpSuccess, bool dbError);
    static void respondWithCAS(GetCouchbaseDocumentByKeyRequest *originalRequest,  std::string couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);
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
        ar & GetAndSubscribe;
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
        ar & GetAndSubscribe;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // GETCOUCHBASEDOCUMENTBYKEYREQUEST_H

