#include "musicfingerssynthesistoolkitcli.h"

#include "musicfingerssynthesistoolkit.h"

MusicFingersSynthesisToolkitCli::MusicFingersSynthesisToolkitCli(QObject *parent)
    : QObject(parent)
{
    MusicFingersSynthesisToolkit *musicFingersSynthesisToolkit = new MusicFingersSynthesisToolkit(this);
    Q_UNUSED(musicFingersSynthesisToolkit) //dgaf
}
