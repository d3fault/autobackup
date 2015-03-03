#ifndef SAMPLESKEYBOARDPIANOPROFILECONFIGURATION_H
#define SAMPLESKEYBOARDPIANOPROFILECONFIGURATION_H

#include <QHash>
#include <QtMultimedia/QSoundEffect>

class SamplesKeyboardPianoProfileConfiguration
{
public:
    QHash<int /*keyboardKey*/, QSoundEffect* /*sample*/> KeyboardKeysToSamplesHash;
};

#endif // SAMPLESKEYBOARDPIANOPROFILECONFIGURATION_H
