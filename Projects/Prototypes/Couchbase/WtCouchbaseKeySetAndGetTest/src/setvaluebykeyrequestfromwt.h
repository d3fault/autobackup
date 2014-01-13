#ifndef SETVALUEBYKEYREQUESTFROMWT_H
#define SETVALUEBYKEYREQUESTFROMWT_H

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>

typedef boost::function<void (std::string,std::string)> SetValueByKeyWtPostCallback;

class SetValueByKeyRequestFromWt
{
public:
    SetValueByKeyRequestFromWt(std::string wtSessionId, SetValueByKeyWtPostCallback setValueByKeyWtPostCallback, std::string couchbaseSetKey, std::string couchbaseSetValue)
        : WtSessionId(wtSessionId), WtPostCallback(setValueByKeyWtPostCallback), CouchbaseSetKey(couchbaseSetKey), CouchbaseSetValue(couchbaseSetValue)
    {}


    SetValueByKeyRequestFromWt()
    {}

    std::string WtSessionId;
    SetValueByKeyWtPostCallback WtPostCallback;
    std::string CouchbaseSetKey;
    std::string CouchbaseSetValue;
    //unsigned int relevantPage; //TODOreq: maybe only/more appropriate for GET'ing, but my instincts tell me it MIGHT apply to both (or none at all lol): basically in the Wt app side of things I should have an enum of "pages" (views) and when the couchbase thread responds via "Post", we should check the value of relevantPage to see if the user is still even on it! I'm not sure that Post would even get activated in that case and should test this out: test by doing a set/get (either or), then using a timeout on the couchbase thread to give my slow human reaction time some time to "change [relevant] pages" and then like litter debug/breakpoints all over the place and see what the fuck happens :)
    //^I have a hunch that "Post" takes care of this for me.. but I only recall it ever saying it can handle WApplication's that are deleted... not anything about "changing pages" (changing a stackwidget index etc etc)

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & WtSessionId;
        ar & WtPostCallback;
        ar & CouchbaseSetKey;
        ar & CouchbaseSetValue;
    }
};

#endif // SETVALUEBYKEYREQUESTFROMWT_H
