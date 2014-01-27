#ifndef STORECOUCHBASEDOCUMENTBYKEYREQUEST_H
#define STORECOUCHBASEDOCUMENTBYKEYREQUEST_H

#include <cstring>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

class AnonymousBitcoinComputingWtGUI;

class StoreCouchbaseDocumentByKeyRequest
{
public:
    enum LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum
    {
        AddMode,
        StoreNoCasMode,
        StoreWithCasMode
    };
    enum WhatToDoWithOutputCasEnum
    {
        DiscardOuputCasMode,
        SaveOutputCasMode
    };
    //save constructor no cas overload
    StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode = AddMode, WhatToDoWithOutputCasEnum whatToDoWithOutputCas = DiscardOuputCasMode);
    //save constructor cas overload
    StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, u_int64_t cas, WhatToDoWithOutputCasEnum whatToDoWithOutputCas = DiscardOuputCasMode);
    //load constructor
    StoreCouchbaseDocumentByKeyRequest();

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseAddKeyInput;
    std::string CouchbaseAddDocumentInput;
    bool LcbStoreModeIsAdd; //LCB_ADD vs. LCB_SET
    bool HasCasInput; //whether or not the following CAS is valid or not
    u_int64_t CasInput;
    bool SaveCasOutput;

    static void respond(StoreCouchbaseDocumentByKeyRequest *originalRequest, bool lcbOpSuccess, bool dbError);
    static void respondWithCas(StoreCouchbaseDocumentByKeyRequest *originalRequest, u_int64_t casOutput, bool lcbOpSuccess, bool dbError);
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

        ar & CouchbaseAddKeyInput;
        ar & CouchbaseAddDocumentInput;
        ar & LcbStoreModeIsAdd;
        ar & HasCasInput;
        if(HasCasInput)
        {
            ar & CasInput;
        }
        ar & SaveCasOutput;
    }
    template<class Archive>
    void load(Archive &ar, const unsigned int version)
    {
        (void)version;
        ar & WtSessionId;

        char AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI];
        ar & boost::serialization::make_binary_object(&AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI);
        memcpy(&AnonymousBitcoinComputingWtGUIPointerForCallback, &AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI-1);

        ar & CouchbaseAddKeyInput;
        ar & CouchbaseAddDocumentInput;
        ar & LcbStoreModeIsAdd;
        ar & HasCasInput;
        if(HasCasInput)
        {
            ar & CasInput;
        }
        ar & SaveCasOutput;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // STORECOUCHBASEDOCUMENTBYKEYREQUEST_H
