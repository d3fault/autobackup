#include "synchronoustodaysadslotgetter.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

SynchronousTodaysAdSlotGetter::SynchronousTodaysAdSlotGetter()
    : ISynchronousLibCouchbaseUser()
{ }
void SynchronousTodaysAdSlotGetter::getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime)
{
    if(!connectToCouchbase())
        return;

    //find the campaign slot index in the cache if it exists
    if(!couchbaseGetRequestWithExponentialBackoff(adSpaceCampaignSlotCacheKey(campaignOwnerUsername, campaignIndex), "campaign slot index cache"))
        return; //TODOreq: some default return such as the "no ads" placeholder? 'return' doesn't mean anything just yet so ima hold off until i figure out how the SUCCESS result will be handled

    string slotIndexToTry = "0"; //as int, cast to string. as string, cast to int. NO WINNING AAAAAAAHHH xD

    if(m_LastOpStatus != LCB_SUCCESS)
    {
        //we can handle the campaign slot index cache not existing (LCB_KEY_ENOENT), but nothing else
        if(m_LastOpStatus != LCB_KEY_ENOENT)
        {
            errorOutput("db error while looking for campaign slot index cache: " + lastErrorString());
        }

    }
    else //LCB_SUCCESS, the campaign slot index cache exists
    {
        ptree pt;
        std::istringstream is(m_LastDocGetted);
        read_json(is, pt);

        slotIndexToTry = pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT);
    }

    //pseudo:
    /*
    do
    {
        //get slot index, return its goods if 'current'

    }while(slot at that index is expired || slot at that index doesn't exist)
    */

    //try to get slot
    while(true) //ok so much for that pseudo :-P
    {
        if(!couchbaseGetRequestWithExponentialBackoff(adSpaceCampaignSlotKey(campaignOwnerUsername, campaignIndex, slotIndexToTry)))
            return;

        if(m_LastOpStatus != LCB_SUCCESS)
            return; //TODOreq: this might mean that there are no purchases for today (further checking into LCB_KEY_ENOENT would confirm this)

        //slot at that date exists

        //is it the current datetime range? could be expired, in which case we +1 slot index and try again. it might also be 'tomorrow' (i'm confused on what i mean by this, but it has something to do with starting at index 0 with no cache)
        ptree pt;
        std::istringstream is(m_LastDocGetted);
        read_json(is, pt);

        //my dick is long long
        long long startDateTime = boost::lexical_cast<long long>(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_START_TIMESTAMP)); //TODOreq: we want to return the expire date time along with the ad itself
        if(currentDateTime >= startDateTime && (currentDateTime < (startDateTime+(3600*24)))) //TODOreq: dynamic 24, perhaps a campaign get just once on first app launch. hardcoding fine for now
        {
            //woot, found todays slot :)

            //now we look up the ad slot filler to get the hover/url/image
            if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH)))
                return;

            //have ad slot filler
            ptree pt2;
            std::istringstream is2(m_LastDocGetted);
            read_json(is, pt);

            const string &imageHoverText = pt2.get<string>(JSON_SLOT_FILLER_HOVERTEXT);
            const string &imageUrl = pt2.get<string>(JSON_SLOT_FILLER_URL);
            const string &imageBase64 = pt2.get<string>(JSON_SLOT_FILLER_IMAGEB64);
            //ez p.z., now back to the higher level design shiz...

            //TODOreq: DO SOMETHING WITH IT/THEM (and don't forget to send expired time too)

            //store the this+1 in the cache so tomorrow will be uber fast
            ptree pt3;
            pt3.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, boost::lexical_cast<string>(boost::lexical_cast<int>(slotIndexToTry)+1));
            std::ostringstream newCacheJsonBuffer;
            write_json(newCacheJsonBuffer, pt3, false);
            couchbaseStoreRequestWithExponentialBackoff(adSpaceCampaignSlotCacheKey(campaignOwnerUsername, campaignIndex), newCacheJsonBuffer.str(), LCB_SET, 0, "cache index updating"); //we don't care if the cache index updating fails, because [see this doc to understand why]


            return; //?????????????
        }
        else
        {
            //not valid time range, either expired or ... actually yea it will only ever be expired because we have to go THROUGH todays in order to get to the hypothetical "tomorrows", and we will stop when we get to today so yea. I guess that means the "less than expired start time" above is not necessary... BUT SHIT MAN I ALREADY WROTE IT SO....

            //expired, +1 slot index and let the while(true) loop do it's magic :)
            slotIndexToTry = boost::lexical_cast<string>(boost::lexical_cast<int>(slotIndexToTry)+1); //my favorite and also WWWWWWWWWRRRRRRRRYYYYYYYYYY (but sure beats a typeless / weakly typed language ANY FUCKING DAY (i complain no matter what (so just ignore me (i can amuse myself (fap fap fap)))))
        }
    }
}
void SynchronousTodaysAdSlotGetter::errorOutput(const string &errorString)
{
    cerr << errorString << endl;
}
