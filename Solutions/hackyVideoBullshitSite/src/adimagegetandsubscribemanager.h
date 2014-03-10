#ifndef ADIMAGEGETANDSUBSCRIBEMANAGER_H
#define ADIMAGEGETANDSUBSCRIBEMANAGER_H

class BoostThreadInitializationSynchronizationKit;

class AdImageGetAndSubscribeManager
{
public:
    AdImageGetAndSubscribeManager();
    startManagingAdImageSubscriptionsAndWaitUntilToldToStop(BoostThreadInitializationSynchronizationKit *threadInitializationSynchronizationKit);
};

#endif // ADIMAGEGETANDSUBSCRIBEMANAGER_H
