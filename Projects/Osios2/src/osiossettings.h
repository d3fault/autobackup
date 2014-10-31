#ifndef OSIOSSETTINGS_H
#define OSIOSSETTINGS_H

//BEGIN COMPILE TIME SETTINGS


//fuck it, stream to any single connection, or every. what exactly would be --
//OH RIGHT, the "green status" of knowing you have 3 nodes online! that's the utility of the bootstrap. Bootstrapping (yellow), At least 2 neighbors (green), less than 2 neighbors (red). STILL I obviously want to keep sending the timeline node stream when there is only 1 connection. So bootstrap is PURELY AESTHETIC (but aesthetics are core part of design. you SHOULD rage if your connection is red, because that means your data is not as safe as it would be if your connection is green)
#define OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE 2
#define OSIOS_NUM_NEIGHBORS_TO_WAIT_FOR_VERIFICATION_FROM_BEFORE_CONSIDERING_A_TIMELINE_NODE_VERIFIED OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE /*this could be it's own define/runtime-option/etc, but KISS for now*/
#define OSIOS_DHT_MIN_TIME_TO_WAIT_BEFORE_RETRYING_CONNECTION_MS (100)
#define OSIOS_DHT_MAX_AMOUNT_OF_TIME_TO_WAIT_BEFORE_CHECKING_LIST_FOR_PEERS_TO_RETRY_CONNECTING_TO_MS (60*1000)
#define OSIOS_DHT_TIME_TO_OVERSHOOT_A_RETRYWITHEXPONENTIALBACKOFF_TIMEOUT_WHEN_WE_WANT_TO_HIT_ON_NEXT_TIMEOUT_MS (5)

#define OSIOS_GUI_LAYOUT_CONTENT_MARGINS 0

#define OSIOS_HUMAN_READABLE_TITLE "OSiOS"


//END COMPILE TIME SETTINGS

//BEGIN RUNTIME/PROFILE SETTINGS


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


//END RUNTIME/PROFILE SETTINGS

#endif // OSIOSSETTINGS_H
