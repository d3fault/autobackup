#ifndef STORECOUCHBASEDOCUMENTBYKEYREQUEST_H
#define STORECOUCHBASEDOCUMENTBYKEYREQUEST_H

#include "../abc2common.h"

#include <sys/types.h>

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#include <string>
#else
#include <cstring>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

class AnonymousBitcoinComputingWtGUI;

class StoreCouchbaseDocumentByKeyRequest
{
public:
    enum LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum
    {
        AddMode,
        SetNoCasMode,
        SetWithCasMode
    };
    enum WhatToDoWithOutputCasEnum
    {
        DiscardOuputCasMode,
        SaveOutputCasMode
    };
    //serialize constructor no cas input overload
    StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseStoreKeyInput, std::string couchbaseStoreDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode = AddMode, WhatToDoWithOutputCasEnum whatToDoWithOutputCas = DiscardOuputCasMode);
    //serialize constructor cas input overload
    StoreCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseStoreKeyInput, std::string couchbaseStoreDocumentInput, u_int64_t cas, WhatToDoWithOutputCasEnum whatToDoWithOutputCas = DiscardOuputCasMode);
    //deserialize constructor
    StoreCouchbaseDocumentByKeyRequest();

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseStoreKeyInput;
    std::string CouchbaseStoreDocumentInput;
    bool LcbStoreModeIsAdd; //LCB_ADD vs. LCB_SET
    bool HasCasInput; //whether or not the following CAS is valid or not
    u_int64_t CasInput;
    bool SaveCasOutput;

    static void respond(StoreCouchbaseDocumentByKeyRequest *originalRequest, bool lcbOpSuccess, bool dbError);
    static void respondWithCas(StoreCouchbaseDocumentByKeyRequest *originalRequest, u_int64_t casOutput, bool lcbOpSuccess, bool dbError);
private:
#ifndef ABC_USE_BOOST_LOCKFREE_QUEUE
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

        ar & CouchbaseStoreKeyInput;
        ar & CouchbaseStoreDocumentInput;
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

        ar & CouchbaseStoreKeyInput;
        ar & CouchbaseStoreDocumentInput;
        ar & LcbStoreModeIsAdd;
        ar & HasCasInput;
        if(HasCasInput)
        {
            ar & CasInput;
        }
        ar & SaveCasOutput;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE
};

#endif // STORECOUCHBASEDOCUMENTBYKEYREQUEST_H
