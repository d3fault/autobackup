#ifndef OSIOSSETTINGS_H
#define OSIOSSETTINGS_H

#include <QString>

class QObject;
class QSettings;

//#define DHT_ID_SETTINGS_KEY "dhtId"
#define LAST_USED_PROFILE_SETTINGS_KEY "lastUsedProfile"
#define AUTO_LOGIN_LAST_USED_PROFILE_ON_BOOTSTRAP_SETTINGS_KEY "autoLoginLastUsedProfileOnBootstrap"
#define ALL_PROFILE_NAMES_LIST_SETTINGS_KEY "allProfileNames"
#define PROFILES_GOUP_SETTINGS_KEY "profiles" /* so it looks like this usually: %settings-dir%/profile/username/ */
#define OSIOS_DATA_DIR_SETTINGS_KEY "dataDir"

class OsiosSettings
{
public:
    static void setSettingsFilenameApplicationWide(const QString &applicationWideSettingsFilename);
    static QSettings *newSettings(/*QObject *parent = 0*/);
private:
    static QString m_ApplicationWideSettingsFilename_OrEmptyIfNoneSpecified;
};

#endif // OSIOSSETTINGS_H
