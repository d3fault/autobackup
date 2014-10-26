#ifndef OSIOSCOMMON_H
#define OSIOSCOMMON_H

#define PROFILES_GOUP_SETTINGS_KEY "profiles"
#define OSIOS_DATA_DIR_SETTINGS_KEY "dataDir"

class MainMenuActivitiesEnum
{
public:
    enum MainMenuActivitiesEnumActual
    {
          TimelineViewActivity = 0
        , WriterActivity = 1
    };
};

//fuck it, stream to any single connection, or every. what exactly would be --
//OH RIGHT, the "green status" of knowing you have 3 nodes online! that's the utility of the bootstrap. Bootstrapping (yellow), At least 2 neighbors (green), less than 2 neighbors (red). STILL I obviously want to keep sending the timeline node stream when there is only 1 connection. So bootstrap is PURELY AESTHETIC (but aesthetics are core part of design. you SHOULD rage if your connection is red, because that means your data is not as safe as it would be if your connection is green)
#define OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE 2

#endif // OSIOSCOMMON_H
