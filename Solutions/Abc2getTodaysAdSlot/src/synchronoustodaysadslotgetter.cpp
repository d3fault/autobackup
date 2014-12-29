#include "synchronoustodaysadslotgetter.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

#define SynchronousTodaysAdSlotGetter_FIRST_NO_CACHE_YET_SLOT_INDEX "0"

SynchronousTodaysAdSlotGetter::SynchronousTodaysAdSlotGetter()
    : ISynchronousLibCouchbaseUser(), m_TodaysAdSlotJson(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER), m_TodaysAdSlotExpirationDate(0)
{ }
bool SynchronousTodaysAdSlotGetter::tryToGetTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime)
{
    if(!connectToCouchbase())
        return false;

    //find the campaign slot index in the cache if it exists
    if(!couchbaseGetRequestWithExponentialBackoff(adSpaceCampaignSlotCacheKey(campaignOwnerUsername, campaignIndex), "campaign slot index cache"))
        return false; //TODOreq: some default return such as the "no ads" placeholder? 'return' doesn't mean anything just yet so ima hold off until i figure out how the SUCCESS result will be handled

    string firstSlotIndexToTry = SynchronousTodaysAdSlotGetter_FIRST_NO_CACHE_YET_SLOT_INDEX; //as int, cast to string. as string, cast to int. NO WINNING AAAAAAAHHH xD

    bool foundCacheDoc = (m_LastOpStatus == LCB_SUCCESS);
    if(!foundCacheDoc)
    {
        //we can handle the campaign slot index cache not existing (LCB_KEY_ENOENT), but nothing else
        if(m_LastOpStatus != LCB_KEY_ENOENT)
        {
            errorOutput("db error while looking for campaign slot index cache: " + lastErrorString());
            return false;
        }
    }
    else //the campaign slot index cache exists
    {
        ptree pt;
        std::istringstream is(m_LastDocGetted);
        read_json(is, pt);

        firstSlotIndexToTry = pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT);
    }
    string slotIndexToTry = firstSlotIndexToTry;

    //pseudo:
    /*
    do
    {
        //get slot index, return its goods if 'current'

    }while(slot at that index is expired || slot at that index doesn't exist)
    */

    while(true) //ok so much for that pseudo :-P
    {
        //try to get slot
        if(!couchbaseGetRequestWithExponentialBackoff(adSpaceCampaignSlotKey(campaignOwnerUsername, campaignIndex, slotIndexToTry), "slot checking if it's todays"))
            return false;

        if(m_LastOpStatus != LCB_SUCCESS)
            return false; //TODOreq: this might mean that there are no purchases for today (further checking into LCB_KEY_ENOENT would confirm this)

        //slot at that index exists

        //is it the current datetime range? could be expired, in which case we +1 slot index and try again. it might also be 'tomorrow' (i'm confused on what i mean by this, but it has something to do with starting at index 0 with no cache)
        ptree pt;
        std::istringstream is(m_LastDocGetted);
        read_json(is, pt);

        //my dick is long long
        long long startDateTime = boost::lexical_cast<long long>(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_START_TIMESTAMP)); //TODOreq: we want to return the expire date time along with the ad itself
        long long expireDateTime = (startDateTime+(3600*24));
        if(currentDateTime >= startDateTime && (currentDateTime < expireDateTime)) //TODOreq: dynamic 24, perhaps a campaign get just once on first app launch. hardcoding fine for now
        {
            //woot, found todays slot :)

            //now we look up the ad slot filler to get the hover/url/image
            if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH), "todays ad slot filler"))
                return false;

            //have ad slot filler
            ptree pt2;
            std::istringstream is2(m_LastDocGetted);
            read_json(is2, pt2);

            ptree todaysAdSlot;
            todaysAdSlot.put(JSON_SLOT_FILLER_HOVERTEXT, pt2.get<string>(JSON_SLOT_FILLER_HOVERTEXT)); //TODOoptional: using json keys in two different docs, fuck it
            todaysAdSlot.put(JSON_SLOT_FILLER_URL, pt2.get<string>(JSON_SLOT_FILLER_URL));
            todaysAdSlot.put(JSON_SLOT_FILLER_IMAGEB64, pt2.get<string>(JSON_SLOT_FILLER_IMAGEB64));
            todaysAdSlot.put(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_EXPIRATION_DATETIME, boost::lexical_cast<string>(expireDateTime));
            todaysAdSlot.put(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_KEY, JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_NOERROR_VALUE);
            std::ostringstream todaysAdSlotJsonBuffer;
            write_json(todaysAdSlotJsonBuffer, todaysAdSlot, false);
            m_TodaysAdSlotJson = todaysAdSlotJsonBuffer.str();
            m_TodaysAdSlotExpirationDate = expireDateTime; //it's in the json, but we use it locally as well...
            //ez p.z., now back to the higher level design shiz...

            //TODOreq: DO SOMETHING WITH IT/THEM (and don't forget to send expired time too)

            //store the this+1 in the cache so tomorrow will be uber fast
            if(slotIndexToTry != firstSlotIndexToTry || (slotIndexToTry == SynchronousTodaysAdSlotGetter_FIRST_NO_CACHE_YET_SLOT_INDEX && !foundCacheDoc)) //only update the cache if it needs to be updated...
            {
                ptree pt3;
                //pt3.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, boost::lexical_cast<string>(boost::lexical_cast<int>(slotIndexToTry)+1));
                pt3.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, slotIndexToTry); //^if we only request once per day then it makes sense to set the cache to "tomorrow" (+1), but if we request it multiple times per day then it makes sense to set it to "today" (no +1). leaving as "today" since it's safer and I still haven't finalized this shit
                std::ostringstream newCacheJsonBuffer;
                write_json(newCacheJsonBuffer, pt3, false);
                couchbaseStoreRequestWithExponentialBackoff(adSpaceCampaignSlotCacheKey(campaignOwnerUsername, campaignIndex), newCacheJsonBuffer.str(), LCB_SET, 0, "cache index updating"); //we don't care if the cache index updating fails, because [see this doc to understand why]
            }


            return true;
        }
        else //+1 and loop back around
        {
            //not valid time range, either expired or ... actually yea it will only ever be expired because we have to go THROUGH todays in order to get to the hypothetical "tomorrows", and we will stop when we get to today so yea. I guess that means the "less than expired start time" above is not necessary... BUT SHIT MAN I ALREADY WROTE IT SO....

            //expired, +1 slot index and let the while(true) loop do it's magic :)
            slotIndexToTry = boost::lexical_cast<string>(boost::lexical_cast<int>(slotIndexToTry)+1); //my favorite and also WWWWWWWWWRRRRRRRRYYYYYYYYYY (but sure beats a typeless / weakly typed language ANY FUCKING DAY (i complain no matter what (so just ignore me (i can amuse myself (fap fap fap)))))
        }
    }
}
string SynchronousTodaysAdSlotGetter::todaysAdSlotJson()
{
    return m_TodaysAdSlotJson;
}
long long SynchronousTodaysAdSlotGetter::todaysAdSlotExpirationDate()
{
    return m_TodaysAdSlotExpirationDate;
}
void SynchronousTodaysAdSlotGetter::errorOutput(const string &errorString)
{
    cerr << errorString << endl;
}
