#include "paginationofviewquery.h"

#include <QScopedPointer>
#include <QDebug>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <libcouchbase/http.h>

using namespace boost::property_tree;

class PaginationOfViewQueryCouchbaseCookieType
{
public:
    explicit PaginationOfViewQueryCouchbaseCookieType(PaginationOfViewQuery *pointerToPaginationOfViewQuery, int pageNumberRequested)
        : PointerToPaginationOfViewQuery(pointerToPaginationOfViewQuery)
        , PageNumberRequested(pageNumberRequested)
    { }

    PaginationOfViewQuery *PointerToPaginationOfViewQuery;
    int PageNumberRequested;
};

PaginationOfViewQuery::PaginationOfViewQuery(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
{ }
void PaginationOfViewQuery::errorOutput(const string &errorString)
{
    qDebug() << QString::fromStdString(errorString);
    emit quitRequested();
}
void PaginationOfViewQuery::httpCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    QScopedPointer<PaginationOfViewQueryCouchbaseCookieType> theCookie(const_cast<PaginationOfViewQueryCouchbaseCookieType*>(static_cast<const PaginationOfViewQueryCouchbaseCookieType*>(cookie)));
    theCookie->PointerToPaginationOfViewQuery->httpCompleteCallback(theCookie->PageNumberRequested, error, resp);
}
void PaginationOfViewQuery::httpCompleteCallback(int pageNum_WithOneBeingTheFirstPage, lcb_error_t error, const lcb_http_resp_t *resp)
{
    //TODOoptimization: pre-format the page/cache before giving to front-end. Is both memory and cpu optimization, memory because don't store a bunch of "null" value [strings], cpu because front-end would have to convert it on each request anyways. We should cache that pre-formatted structure

    //TODOreq: cache page result
    //TODoreq: handle json errors both in backend and frontend. backend=don't cache. frontent=display error message. perhaps for the frontend to know if it's an error or not could rely on whether or not we pass him an empty string
    qDebug() << "httpCompleteCallback";
    if(error != LCB_SUCCESS)
    {
        qDebug() << "httpCompleteCallback has error";
        emit quitRequested();
        return;
    }
    QByteArray pathByteArray(resp->v.v0.path, resp->v.v0.npath);
    QString pathString(pathByteArray);
    qDebug() << pathString;
    QByteArray jsonPageContentsByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonPageContentsString(jsonPageContentsByteArray);
    emit finishedQueryingPageOfView(jsonPageContentsString);

    //cache the docid and startkey stuff! TODOreq: I also want to cache the page CONTENTS, but for now I'm working on just the efficient skipping (they both use the same "cache", just differently)
    ptree pt;
    std::string jsonPageContentsStdString = jsonPageContentsString.toStdString();
    std::istringstream is(jsonPageContentsStdString);
    read_json(is, pt);
    boost::optional<std::string> errorMaybe = pt.get_optional<std::string>("error");
    if(errorMaybe.is_initialized())
    {
        qDebug() << "json had error set";
        //TODOreq: error, this should be handled a little sooner actually (emit empty string on error)
        return;
    }
    const ptree &rowsPt = pt.get_child("rows");
    int numRowsOnPage = rowsPt.size();
    if(numRowsOnPage > 0)
    {
        //TODOreq: check numrows > 0 sooner too, emit empty string if 0. Or perhaps differentiate from error as "page not found", but whatever idk
        //TODOreq: I'm not sure if I should cache the last docid [for using as startkey_docid] if the page doesn't contain the max amount of 'items per page'. For example on the very last page, only 5 of the 10 items might be shown, so uhhhh (mind=exploded). Idk tbh, but yea figure it out...

        const ptree &lastItem = rowsPt.back().second;
        const std::string &lastDocId = lastItem.get<std::string>("id" /* docid, that is */);
        const std::string &lastKey = lastItem.get<std::string>("key" /* emitted view key, username in this case*/);
        qDebug() << "Page" << pageNum_WithOneBeingTheFirstPage << "is caching last doc id:" << QString::fromStdString(lastDocId) << "and last key:" << QString::fromStdString(lastKey);
        CACHED_VIEW_QUERY_PAGES_HASH_VALUE_TYPE newCacheItem = qMakePair(lastDocId, lastKey);
        m_CachedPagesAndTheirLastDocIdsAndLastKeys.insert(pageNum_WithOneBeingTheFirstPage, newCacheItem);
    }
}
void PaginationOfViewQuery::initializePaginationOfViewQuery()
{
    if(!connectToCouchbase())
    {
        emit paginationOfViewQueryInitialized(false);
        return;
    }
    lcb_set_http_complete_callback(m_Couchbase, PaginationOfViewQuery::httpCompleteCallbackStatic);
    emit paginationOfViewQueryInitialized(true);

    //kind of a hack, so that when the gui loads for the first time we get the first page right away
    queryPageOfView(1);
}
void PaginationOfViewQuery::queryPageOfView(int pageNum_WithOneBeingTheFirstPage)
{
    static const int itemsPerPage = 2;

    lcb_http_cmd_t queryPageOfViewCmd;
    queryPageOfViewCmd.version = 0;

    //re: startkey_docid and caching view queries. the "previous page" (used to determine startkey_docid) must be in the cache to use that feature, but the "next page" clearly isn't (otherwise we'd return it duh TODOreq). Err my point is that maybe the db has changed so that starkey_docid isn't pointing to the right page "SYNC" point. Shit even hypothetically (but not in Abc2?), the startkey_docid could have been deleted... and then what? Of course it's better to just put deleted=true than to delete it for realz (db 101)... I don't really know what I'm getting at here... just thinking out loud. Hmm, what if the previous page is in the cache but is expired? Should we still use it's startkey_docid? Should we use the "closest" startkey_docid that isn't expired(cache-wise), even if it's from 5 'pages' previous? Kind of makes sense to do that. How/when should I expire the cached view queries/pages? Lazily is most efficient CPU-wise, but sheeeit has the potential to eat up assloads of RAM. QCache jumps out at me ofc, but Abc2 for some reason utterly refuses to use Qt (why? I keep asking him but he won't tell me).
    //Awww shit does startkey_docid even apply when reduce is in the equation? Methinks not, fml. If I did a "all ad campaigns by all users" (or even just "all users") thing that used pagination, then I could use startkey_docid (since there'd be no reduce), but man I really want "all users with at least one ad campaign" :(, and I don't know how to do it without a reduce :(...

    //RANDOM/semi-OT: Since my ad campaign indexes increment, my VIEW could only emit the 0th one and then blamo no need to reduce!!! This breaks if/when I enable ad campaign deleting via delete=0, (NOPE: but I could account for that by emitting the first ad campaign index seen [oh wait nvm there's no way to know whether or not]...). I think this is a fine solution for now :-/

    std::string viewPath = "_design/dev_AllUsersWithAtLeastOneAdCampaign/_view/AllUsersWithAtLeastOneAdCampaign?stale=false&limit="; //once I upgrade to couchbase 3.0, I'll use stale=ok :-D
    viewPath += QString::number(itemsPerPage).toStdString(); //limit

#if 0 //TODOreq: inefficient. use starkey_docid
    int skip = (pageNum_WithOneBeingTheFirstPage-1)*itemsPerPage;
    if(skip > 0)
    {
        viewPath += "&skip=";
        viewPath += QString::number(skip).toStdString(); //skip
    }
#endif

#if 0 //was just a small hardcoded test (worx)
    //adSpaceCampaign_d3fault_0
    if(pageNum_WithOneBeingTheFirstPage > 1)
    {
        viewPath += "&starkey_docid=\"adSpaceCampaign_d3fault_0\"&startkey=\"d3fault\"&skip=1";
    }
#endif

    if(pageNum_WithOneBeingTheFirstPage > 1) //there is definitely not a [previous] page 0 in cache (since we start at page 1), and also the optimization doesn't even apply for the first page anyways lol (nothing to skip)
    {
        /*
        if previous page is in cache and not expired
        {
            use previous page's last docid as startkey_docid and last key as startkey, then do skip=1 --- and that's about it.... but heh the pseudo is much simpler xD
        }
        */

        //I have a few choices with how to expire cache items. I think I should be proactive about it since far back pages (and there could be lots of them) may be access infrequently and therefore we might run out of memory if we aren't proactive, which means I DON'T need to check expirations here/now: if it's in the cache, it isn't expired. The "cache expiry timer" timeout should be (CACHE_LENGTH / 2) btw TODOreq, which yea can be still inaccurate by ~(CACHE_LENGTH + (CACHE_LENGTH / 2)), but whatever~
        //this is all a complete fucking waste of time if Abc2 never allows the public to create ad campaigns btw :-), since there'd just be "d3fault" (which I should just hardcode at that point) xD xD... but yea I mean I guess I'm still learning about views/reduces/etc in a hands on manner...

        CACHED_VIEW_QUERY_PAGES_HASH_VALUE_TYPE previousPageInCacheMaybe = m_CachedPagesAndTheirLastDocIdsAndLastKeys.value(pageNum_WithOneBeingTheFirstPage-1);
        if(previousPageInCacheMaybe.first != "" /*checking instead if second is not empty would work just as well*/)
        {
            qDebug() << "previous page existed, so we're using it's last docid as our startkey_docid";
            //previous page is in cache, so we can get the page requested pretty efficiently. TODOmb: use a map instead of a hash, and then get simply the "previous map entry" (since sorted by page, we know it's BEFORE the page we are requesting)... which may or may not be exactly page-1. But it would still be an optimization to get page-2 or page-3 if, say, we're requesting page 999999. KISS and just a hash requiring page-1 for now (otherwise optimization isn't used (TODOreq: if the optimization isn't used (regardless of hash or map), we should fall back to the basic skip+limit strategy duh)). Hash is more efficient to lookup in, map allows us to easily get the "last page in cache before the page we want" easier (getting it in qhash would require iterating/decrementing/checking-if-exists, yuck) -- they both have tradeoffs idfk
            //TODOoptimization: if using the above described "map" strategy, we could amplify the "skip" by however much we need to. If page-2 is cached, then we skip over page-1 (even though page-1 isn't cached) simply because we know how many are on page-1. In fact, this optimization might be necessary idfk. TODOreq: test jumping to pages directly ofc, not using the startkey_docid needs to function as well. Be careful with off by ones when doing that optimization xD
            viewPath += "&starkey_docid=\"" + previousPageInCacheMaybe.first + "\"&startkey=\"" + previousPageInCacheMaybe.second + "\"&skip=1";
        }
        else
        {
            //regular inefficient skip
            viewPath += "&skip=";
            viewPath += QString::number((pageNum_WithOneBeingTheFirstPage-1)*itemsPerPage).toStdString(); //skip
        }
    }

    queryPageOfViewCmd.v.v0.path = viewPath.c_str();
    queryPageOfViewCmd.v.v0.npath = strlen(queryPageOfViewCmd.v.v0.path);
    queryPageOfViewCmd.v.v0.body = NULL;
    queryPageOfViewCmd.v.v0.nbody = 0;
    queryPageOfViewCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    queryPageOfViewCmd.v.v0.chunked = 0;
    queryPageOfViewCmd.v.v0.content_type = "application/json";
    lcb_error_t error = lcb_make_http_request(m_Couchbase, new PaginationOfViewQueryCouchbaseCookieType(this, pageNum_WithOneBeingTheFirstPage), LCB_HTTP_TYPE_VIEW, &queryPageOfViewCmd, NULL);
    //libcouchbase_make_couch_request(instance, NULL, path, npath, NULL, 0, LBCOUCHBAESE_HTTP_METHOD_GET, 1);
    if (error != LCB_SUCCESS)
    {
        qDebug() <<  "Failed to query view" << QString::fromStdString(lcb_strerror(m_Couchbase, error));
        emit quitRequested();
        return;
    }
    lcb_wait(m_Couchbase);
}
