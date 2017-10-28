#include "libstk_loopaudiofileandmodifyitspitchusingaslider.h"

#include <QCoreApplication>

#include "objectonthreadgroup.h"
#include "audiooutput.h"

LibStk_LoopAudioFileAndModifyItsPitchUsingAslider::LibStk_LoopAudioFileAndModifyItsPitchUsingAslider(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<stk::StkFloat>("stk::StkFloat");

    ObjectOnThreadGroup *threadGroup = new ObjectOnThreadGroup(this);
    threadGroup->addObjectOnThread<AudioOutput>("handleAudioOutputReadyForConnections");
    threadGroup->doneAddingObjectsOnThreads();
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAslider::handleAudioOutputReadyForConnections(QObject *audioOutputAsQObject)
{
    if(AudioOutput *audioOutput = qobject_cast<AudioOutput*>(audioOutputAsQObject))
    {
        m_Gui.reset(new LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget());

        connect(m_Gui.data(), &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::changePitchShiftRequested, audioOutput, &AudioOutput::changePitchShift);
        connect(m_Gui.data(), &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::changePitchShiftMixAmountRequested, audioOutput, &AudioOutput::changePitchShiftMixAmount);

        m_Gui->resize(100, 300);
        m_Gui->show();

        connect(qApp, &QCoreApplication::aboutToQuit, audioOutput, &AudioOutput::stopAudioOutput);
        QMetaObject::invokeMethod(audioOutput, "startAudioOutput", Qt::QueuedConnection);
    }
    else
    {
        qFatal("qobject was not audiooutput");
    }
}
