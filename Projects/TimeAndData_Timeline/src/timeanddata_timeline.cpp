#include "timeanddata_timeline.h"

#include <QDateTime>

//TODOreq: there is no delete, only append
TimeAndData_Timeline::TimeAndData_Timeline(QObject *parent)
    : QObject(parent)
{ }
void TimeAndData_Timeline::appendJsonObjectToTimeline(const QJsonObject &data)
{
    //TO DOnereq(read huge comment below): should I have time passed in to me or should I determine it myself here/now?
    QDateTime currentDateTimeUtc = QDateTime::currentDateTimeUtc(); //TODOreq: _always_ UTC in backend, then convert-on-demand to timezones when showing user
    //TODOreq: should I factor in a cryptographic hash of the data so that time/key collisions don't overwrite?
    //I'm kinda thinking a "one file per timeline entry" fs heirarchy, in which case colliding timestamps could be handled in any number of ways (0000.bin, 0001.bin (therein, time+data serialized), etc). if I want to get fancy I could do year/month folderization and put the cryptographic hash in the filename. folderization is really just an optimization though (a worthwhile one if you ask me). fuck I need to DECIDE. do I want to allow 2 different items to have the same timestamp? of fucking course. which strat do I want? blah there are pros an cons of each, but worse there are SERIOUSLY HUGE IMPLICATIONS of each (unless I abstract it properly).
    //do I want to have mid-timeline insertions, or only appends? I think yes appends only except MAYBE in some special admin/FIXING-SOMETHING-BROKEN/dangerous mode? so that means WE are the ones to gen the timestamp, ok changing my api now to reflect that
    //it's STILL possible to get dupe timestamps of course. you know what fuck it I like year/month/day folderization, then numMSecElapsedToday_sha512orwhatever.bin -- ex: 2017/9/23/69420_09f9deadbeefbabesha512hex.bin
    //MAYBE, if after analyzing data the type can be DETERMINED to be plaintext, we use a .txt extension for it and if not a .bin extension? or we can use a .json if determined to be .json? oh where the fuck does it end??? binary/text is one thing, but nah json nah (but then again json is "special" as it's my internal type etc, so maybe ONE exception for json. bin=unknown, txt=txt-not-json, json=json). wait no it is ALWAYS json. we task the USER OF this lib with being json-safe, b64-encoding if needed. so yea there is only ONE extension type xD: .json <3. note that even though it's json and human readable, it's still intended to be READ-ONLY (modifying would invalidate the sha512!! use StupidKeyValueContentTracker if you want to "MODIFY" (revise) a "file". TODOreq: there is a data race maybe if 2+ StupidKeyValueContentTracker instances share a Timeline instance and call append simultaneously. one of them might "mutate" the other mutation (rather than the state before it), which is uninntended. TODOreq: we need a "appendAtExactlyThisPositionOrFail" (the position is polled from Timeline, just before commit. so it will almost always go through, but the occassional data races are prevented! we just error out, possibly retrying (now instead mutating the mutation (or rather, it's state after applied) that JUSt beat us to the append) depending on the intent and if there are conflicts)
    //man this blew up in my face, I'm glad it was kept as simple as it is though :). just time and data :)
}
