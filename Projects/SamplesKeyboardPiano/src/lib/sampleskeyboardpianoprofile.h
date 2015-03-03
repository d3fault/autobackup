#ifndef SAMPLESKEYBOARDPIANOPROFILE_H
#define SAMPLESKEYBOARDPIANOPROFILE_H

#include <QString>
#include <QDir>
#include <QList>

#include "sampleskeyboardpianoprofileconfiguration.h"

class SamplesKeyboardPianoProfile
{
public:
    QString Name;
    QDir BaseDir;
    QList<SamplesKeyboardPianoProfileConfiguration> Configurations;
    SamplesKeyboardPianoProfileConfiguration CurrentConfiguration;
};
Q_DECLARE_METATYPE(SamplesKeyboardPianoProfile)

#endif // SAMPLESKEYBOARDPIANOPROFILE_H
