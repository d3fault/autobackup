#ifndef HVBSSHARED_H
#define HVBSSHARED_H

#define HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/AirborneVideos"
#define HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER "/MyBrain"
#define HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_TIMELINE \
    HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER "/Timeline"
#define HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN \
    HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER "/Browse"
#define HVBS_NO_HTML_MEDIA_OR_ERROR(mediaType) "Either your browser is a piece of shit and doesn't support HTML5 " #mediaType " (You should use Mozilla Firefox), or there was an error establishing a connection to the " #mediaType " stream"
#define HVBS_NO_HTML5_VIDEO_OR_ERROR HVBS_NO_HTML_MEDIA_OR_ERROR(video)
#define HVBS_NO_HTML5_AUDIO_OR_ERROR HVBS_NO_HTML_MEDIA_OR_ERROR(audio)
#define HVBS_DOWNLOAD_LOVE \
    "Download " \
    "/ Save " \
    "/ Fukken Save " \
    "/ Keep " \
    "/ Steal " \
    "/ Borrow " \
    "/ Use " \
    "/ Absorb " \
    "/ Own " \
    "/ Assimilate" \
    "/ Incorporate " \
    "/ Memorize " \
    "/ Multiply " \
    "/ Archive " \
    "/ Persist " \
    "/ Cache " \
    "/ Hoard " \
    "/ Receive " \
    "/ Replicate " \
    "/ Reproduce" \
    "/ Grab " \
    "/ Get " \
    "/ Copy"
//having way too much fun with this

#define HVBS_MY_BRAIN_PUBLIC_FILES_TORRENT_FILENAME "MyBrain-PublicFiles-Uncompressed.torrent"
#define HVBS_MY_BRAIN_PUBLIC_FILES_TORRENT_PUBLIC_RESOURCE "/" HVBS_MY_BRAIN_PUBLIC_FILES_TORRENT_FILENAME
#define HVBS_MY_BRAIN_2014_SUPPLEMENT_TORRENT_FILENAME "MyBrain-2014-Supplement.torrent"
#define HVBS_MY_BRAIN_2014_SUPPLEMENT_TORRENT_PUBLIC_RESOURCE "/" HVBS_MY_BRAIN_2014_SUPPLEMENT_TORRENT_FILENAME
#define HVBS_MY_BRAIN_PRIVATE_FILES_TORRENT_FILENAME "MyBrain-PrivateFiles-Uncompressed.torrent"
#define HVBS_MY_BRAIN_PRIVATE_FILES_TORRENT_PUBLIC_RESOURCE "/" HVBS_MY_BRAIN_PRIVATE_FILES_TORRENT_FILENAME

#endif // HVBSSHARED_H
