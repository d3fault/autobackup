#include "sampleskeyboardpianogui.h"

#include <QCoreApplication>

#include "sampleskeyboardpiano.h"
#include "sampleskeyboardpianowidget.h"

SamplesKeyboardPianoGui::SamplesKeyboardPianoGui(QObject *parent)
    : QObject(parent)
    , m_SamplesKeyboardPiano(new SamplesKeyboardPiano(this))
{
    connectSamplesKeyboardPianoBackendToProfileManagerFrontendAndShow();
    //connectSamplesKeyboardPianoBackendToFrontend(samplesKeyboardPiano);
}
void SamplesKeyboardPianoGui::connectSamplesKeyboardPianoBackendToProfileManagerFrontendAndShow()
{
    m_SamplesKeyboardPianoProfileManagerWidget.reset(new SamplesKeyboardPianoProfileManagerWidget());
    connect(m_SamplesKeyboardPiano, SIGNAL(profilesProbed(QList<SamplesKeyboardPianoProfile>)), m_SamplesKeyboardPianoProfileManagerWidget.data(), SLOT(handleProfilesProbed(QList<SamplesKeyboardPianoProfile>)));
    connect(m_SamplesKeyboardPianoProfileManagerWidget.data(), SIGNAL(accepted()), this, SLOT(handleSamplesKeyboardPianoProfileManagerProfileSelected()));
    connect(m_SamplesKeyboardPianoProfileManagerWidget.data(), SIGNAL(rejected()), qApp, "quit", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_SamplesKeyboardPiano, "probeProfiles");
}
void SamplesKeyboardPianoGui::connectSamplesKeyboardPianoBackendToFrontendAndShow(SamplesKeyboardPianoProfile profile)
{
    m_SamplesKeyboardPianoWidget.reset();
    QMetaObject::invokeMethod(m_SamplesKeyboardPiano, "selectProfile", Q_ARG(SamplesKeyboardPianoProfile, profile));
    connect(m_SamplesKeyboardPianoWidget.data(), SIGNAL(alphabeticalKeyPressDetected(int,bool)), m_SamplesKeyboardPiano, SLOT(drum(int,bool)));
    m_SamplesKeyboardPianoWidget->show();
}
void SamplesKeyboardPianoGui::handleSamplesKeyboardPianoProfileManagerProfileSelected()
{
    connectSamplesKeyboardPianoBackendToFrontendAndShow(m_SamplesKeyboardPianoProfileManagerWidget.data()->selectedProfile());
    m_SamplesKeyboardPianoProfileManagerWidget.data()->hide();
}


