#ifndef SETVALUEBYKEYREQUESTFROMWT_H
#define SETVALUEBYKEYREQUESTFROMWT_H

#include "wtkeysetandgetwidget.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

//template class here? hmmm... :-P...
class SetValueByKeyRequestFromWt
{
public:
    //save constructor
    SetValueByKeyRequestFromWt(std::string wtSessionId, WtKeySetAndGetWidget *pointerToWtKeySetAndGetWidget, std::string couchbaseSetKey, std::string couchbaseSetValue)
        : WtSessionId(wtSessionId), WtKeySetAndGetWidgetPointerForCallback(pointerToWtKeySetAndGetWidget), CouchbaseSetKey(couchbaseSetKey), CouchbaseSetValue(couchbaseSetValue)
    { }
    //load constructor
    SetValueByKeyRequestFromWt()
    { }

    std::string WtSessionId;
    //SetValueByKeyWtPostCallback WtPostCallback; //maybe a handle_t from boost IPC (offset_ptr) can be serialized??? Or maybe I can use hacky boost wait conditions to get both threads (err, see below about 10+ threads WTF) to be able to point to the same "pointer" in the shared memory segment that handle_t points to??? but then there's still the platform independent pointer sizing matter, guh! MAYBE I SHOULD JUST #ifdef 64-bit int64 * #else int32 * #endif and be done with it! Still hacky to pass an address as integer around like that, BUT WServer::post does verification anyways so fuuuuuuck it
    //boost::interprocess::managed_shared_memory::handle_t SetValueByKeyWtPostCallback;

    //unsigned char LengthOfAddressToWApplication; //256-bit addresses, future proof enough rite?
    //char *AddressToWApplication;
    WtKeySetAndGetWidget *WtKeySetAndGetWidgetPointerForCallback;
    static const unsigned char LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback = sizeof(WtKeySetAndGetWidget*) + 1;
    std::string CouchbaseSetKey;
    std::string CouchbaseSetValue;
    //unsigned int relevantPage; //TODOreq: maybe only/more appropriate for GET'ing, but my instincts tell me it MIGHT apply to both (or none at all lol): basically in the Wt app side of things I should have an enum of "pages" (views) and when the couchbase thread responds via "Post", we should check the value of relevantPage to see if the user is still even on it! I'm not sure that Post would even get activated in that case and should test this out: test by doing a set/get (either or), then using a timeout on the couchbase thread to give my slow human reaction time some time to "change [relevant] pages" and then like litter debug/breakpoints all over the place and see what the fuck happens :)
    //^I have a hunch that "Post" takes care of this for me.. but I only recall it ever saying it can handle WApplication's that are deleted... not anything about "changing pages" (changing a stackwidget index etc etc)
    //^^see WApplication::bind, even though that still isn't exactly what I'm talking about (but it could be if I deleted when changing pages)

private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive &ar, const unsigned int version) const
    {
        (void)version;
        ar & WtSessionId;

        char AddressToWtKeySetAndGetWidgetAsString[LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback];
        memcpy(&AddressToWtKeySetAndGetWidgetAsString, &WtKeySetAndGetWidgetPointerForCallback, LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback);
        AddressToWtKeySetAndGetWidgetAsString[LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback-1] = '\0'; //????? seen this before, idk if i need/want it...
        ar & boost::serialization::make_binary_object(&AddressToWtKeySetAndGetWidgetAsString, LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback);

        ar & CouchbaseSetKey;
        ar & CouchbaseSetValue;
    }
    template<class Archive>
    void load(Archive &ar, const unsigned int version)
    {
        (void)version;
        ar & WtSessionId;

        char AddressToWtKeySetAndGetWidgetAsString[LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback];
        ar & boost::serialization::make_binary_object(&AddressToWtKeySetAndGetWidgetAsString, LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback);
        memcpy(&WtKeySetAndGetWidgetPointerForCallback, &AddressToWtKeySetAndGetWidgetAsString, LengthOfAddressToWtKeySetAndGetWidgetPointerForCallback-1);

        ar & CouchbaseSetKey;
        ar & CouchbaseSetValue;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // SETVALUEBYKEYREQUESTFROMWT_H
