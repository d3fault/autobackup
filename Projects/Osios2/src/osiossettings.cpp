#include "osiossettings.h"

#include <QSettings>

//simple wrapper around QSettings, allowing me to specify a different settings file that takes effect application-wide. I could additionally wrap the above defines/keys into getters/setters, but won't for now
QString OsiosSettings::m_ApplicationWideSettingsFilename_OrEmptyIfNoneSpecified; //if none specified, we just use the default QSettings constructor, which usually gives us ~/.config/%AppName%/, etc
void OsiosSettings::setSettingsFilenameApplicationWide(const QString &applicationWideSettingsFilename)
{
    m_ApplicationWideSettingsFilename_OrEmptyIfNoneSpecified = applicationWideSettingsFilename;
}
QSettings *OsiosSettings::newSettings(/*QObject *parent*/)
{
    if(!m_ApplicationWideSettingsFilename_OrEmptyIfNoneSpecified.isEmpty() /* TODOmb: is this thread safe? since it's static idfk. fuck it for now */)
    {
        return new QSettings(m_ApplicationWideSettingsFilename_OrEmptyIfNoneSpecified, QSettings::IniFormat /*, parent*/);
    }
    return new QSettings(/*parent*/);
}
