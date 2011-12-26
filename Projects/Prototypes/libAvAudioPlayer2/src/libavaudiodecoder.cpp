#include "libavaudiodecoder.h"

libAvAudioDecoder::libAvAudioDecoder(QObject *parent) :
    QObject(parent)
{
}
void libAvAudioDecoder::handleNewDataAvailable(QByteArray newData)
{
    //queue it to be decoded later? maybe "de-Stream" it via libav and then queue it to be decoded later? (so we are ONLY processing audio stream)
    //in any case, we don't need to mutex our buffer since this slot is called from this thread, yay finally a payoff for doing getFrame() as a slot lawl (pause/stop still apply but i never code those in prototypes fuck the police)
}
