#include "documenttimelinedb.h"

#include <QSettings>
#include <QCryptographicHash>

#define DocumentTimeline_ORGANIZATION "DocumentTimelineOrganization"
#define DocumentTimeline_QSettingsApplication_aka_DbName "DocumentTimeline"

DocumentTimelineDb::DocumentTimelineDb(QObject *parent)
    : QObject(parent)
    , m_Settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimeline_QSettingsApplication_aka_DbName, this))
{ }
void DocumentTimelineDb::addDocToDb(QByteArray docToAddToDb, void *userData) //tempted to make userData have a default value of 0 (so it'd be optional), but I think that fucks up my Qt5-style signal/slot connections :(. I'm now thinking about how userData should probably be added to every api call of Rpc Generator (as the last arg)
{
    //once there's couchbase integration, all we'd do here is use libevent2 and lockfree::queue to tell the couchbase thread to schedule the LCB_ADD. we'd return before the couchbase thread even begins attempting to schedule that LCB_ADD (let alone perform it)
    m_Settings->setValue(QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Sha1).toHex(), docToAddToDb);
    //then, finally, when couchbase finished the LCB_ADD and everything goes ok:
    emit addDocToDbFinished(false, true, userData);
}
