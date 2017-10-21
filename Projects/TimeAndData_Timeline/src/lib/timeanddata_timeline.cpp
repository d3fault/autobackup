#include "timeanddata_timeline.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QCoreApplication> //for QSettings global config
#include <QSettings> //TODOmb: migrate away from the simplicity of a QSettings-based application database?
#include <QCryptographicHash>
#include <QStringList>

#include <QDebug>

#define TimeAndData_Timeline_GROUP "TimeAndData_TimelineGroup"

//note: there is no delete, only append
//I was originally going to make "data" be a QString, but I think I'm changing that now to be a QJsonObject. anything that can be a string can be a json object, so it's not a huge requirement. it is just easier to work with and requires less serialization/deserialization when working with the "users of" this lib. serialize ONCE, parse ONCE. if data was QString then we'd need to serialize [at least] twice and parse [at least] twice. I hope this decision doesn't come back to bite me in the ass...
TimeAndData_Timeline::TimeAndData_Timeline(QObject *parent)
    : QObject(parent)
{
    QCoreApplication::setOrganizationName("TimeAndData_TimelineOrganization");
    QCoreApplication::setOrganizationDomain("TimeAndData_TimelineDomain");
    QCoreApplication::setApplicationName("TimeAndData_Timeline");
    QSettings::setDefaultFormat(QSettings::IniFormat); //TODOreq: search EVERY project I've coded for the use of the above 3 configs (setOrgName, setDomainName, setAppName) and make sure that this setDefaultFormat is always called there too (unless the app doesn't warrant it, but I think most do. Ini files are just much more portable than "registrty edits" on that one gross operating system. Hell I think it should default to Ini format on all platforms and the Native formats should be opted INTO (since they [can] _SUCK_) TODOmb: file bugreport to default to Ini (or pull request)
}
void TimeAndData_Timeline::readAndEmitAllTimelineEntries()
{
    //TODOoptimization: it's more memory efficient to read one entry at a time (so one entry/emit per slot invoke -- and yea the slot takes some arg specifying which), but that requires breaking up the code and stuff... so it should come later. KISS for now, even if over-simplified and inefficient
    QSettings settings;
    settings.beginGroup(TimeAndData_Timeline_GROUP);
    QStringList childKeys = settings.childKeys();
    AllTimelineEntriesType allTimelineEntries;
    for(QStringList::const_iterator it = childKeys.constBegin(); it != childKeys.constEnd(); ++it)
    {
        QString timeAndDataJsonString = settings.value(*it).toString();
        QByteArray timeAndDataJsonBA = timeAndDataJsonString.toUtf8();
        QJsonParseError jsonParseError;
        QJsonDocument timeAndDataJsonDoc = QJsonDocument::fromJson(timeAndDataJsonBA, &jsonParseError);
        if(jsonParseError.error != QJsonParseError::NoError)
        {
            //TODOreq: handle parse errors good. we should never see this unless the user manually modified the json
            qDebug() << "";
            qDebug() << timeAndDataJsonString;
            qDebug() << "";
            qDebug() << "json parse error: " << jsonParseError.errorString();
            qFatal("json parse error");
            return;
        }
        QJsonObject timeAndDataJson = timeAndDataJsonDoc.object();
        bool convertOk = false;
        qint64 time = timeAndDataJson.value(TimeAndData_Timeline_JSONKEY_TIME).toString().toLongLong(&convertOk);
        if(!convertOk)
        {
            //TODOreq: handle parse errors good. we should never see this unless the user manually modified the json
            qFatal("timestamp parse error");
            return;
        }
        const QJsonObject &data = timeAndDataJson.value(TimeAndData_Timeline_JSONKEY_DATA).toObject();
        allTimelineEntries.insert(time, data);
    }
    settings.endGroup();
    emit finishedReadingAllTimelineEntries(allTimelineEntries);
}
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

    //I've thought more on this and decided not to do year/month/day/msecElapsedSinceStartOfDay.json, yet
    //nor will I use a cryptographic hash, yet
    //I'm going to do something quick and simple and PREDICT that I will refactor it when I implement readKey and/or readTimeline(what to use as input for timeline? (this will "come to me" when I'm coding readKey probably))

    QJsonObject timeAndDataJsonObject;
    timeAndDataJsonObject.insert(TimeAndData_Timeline_JSONKEY_TIME, QString::number(currentDateTimeUtc.toMSecsSinceEpoch())); //fuck human readable time formats, because without a timezone (as is often the case), they are ambiguious (yes I know Qt provides a format with timezone, but if we parse human readable then we INVITE people to change the date format as input and it will "work on their machine" without the timezone -_-. note: we store the number as a string because json can only handle int (not [necessarily] int64) and double
    timeAndDataJsonObject.insert(TimeAndData_Timeline_JSONKEY_DATA, data);
    QJsonDocument jsonDoc(timeAndDataJsonObject);
    QByteArray jsonByteArray = jsonDoc.toJson(QJsonDocument::Compact/*TODOreq: Indented if ever moved out of QSettings*/);
    QByteArray hash = QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Sha3_256);
    QString hashOfTimeAndData(hash.toHex());
    QSettings settings;
    settings.beginGroup(TimeAndData_Timeline_GROUP);
    settings.setValue(hashOfTimeAndData, QString(jsonByteArray) /*nope (works but FUGLY/binary): data.toVariantMap()*/);
    settings.endGroup();
    emit finishedAppendingJsonObjectToTimeline(true);
}
