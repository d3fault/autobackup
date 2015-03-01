#include "drummachine.h"

#include <QtMultimedia/QSoundEffect>

DrumMachine::DrumMachine(QObject *parent)
    : QObject(parent)
{
    QSoundEffect *soundEffect = new QSoundEffect(this);
    soundEffect->setSource(QUrl::fromLocalFile());
}
void DrumMachine::drum(Qt::Key key)
{
    QSoundEffect *soundEffect = m_KeysToSoundEffectsHash.value(key);
    soundEffect->play();
}
