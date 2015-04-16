#include "musicfingerssynthesistoolkit.h"

#include <QCoreApplication>
#include <QDateTime>

#include "musicfingersserialportintegration.h"

#define MIN_PLUCKED_TOGGLE_MS 30 //sort of like debounce, but for determining whether or not to send a NoteOff immediately after a NoteOn

//TODOreq: the 'plucked' vs. 'continuous' instruments need to be handled differently. obviously, the rate at which i bend my finger should change the velocity/attack (or whatever it's called) of the pluck. maybe it also affects the velocity/attack of the continuous ones too (i hadn't really planned on changing that at all, but only changing the frequency/pitch/whatever-its-called(are they the same thing? /musicnoob)). ALTERNATIVELY, I could simply have the 'plucked' ones be [re-]plucked once every N ms automatically (idfk)
//TODOoptional: easing curves
//TODOoptional: saxofone mode (one finger controls velocity or some such, the other fingers 'cover the holes')
//decided that since this is most likely going to be a hacked together piece of shit, it should be a prototype (but in all fairness, life itself is a hacked together piece of shit [and there's still no `proper`)
MusicFingersSynthesisToolkit::MusicFingersSynthesisToolkit(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::LeftPinky_A0, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::LeftRing_A1, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A2_LeftMiddle, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A3_LeftIndex, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A4_LeftThumb, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A5_RightThumb, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A6_RightIndex, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A7_RightMiddle, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A8_RightRing, 0);
    m_FingersPluckedAt_OrZeroIfNotPlucked.insert(Finger::A9_RightPinky, 0);
}
int MusicFingersSynthesisToolkit::channelAkaGroupToPluckFrequency(int channelAkaGroup)
{
    return (35+(channelAkaGroup*5));
}
void MusicFingersSynthesisToolkit::switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(int channelAkaVoiceGroup, int instrumentIndex)
{
    m_StdOut << "ProgramChange 0 " << channelAkaVoiceGroup << " " << instrumentIndex << endl;
    //m_StdOut << "NoteOn 0 " << channelAkaVoiceGroup << " 64 64" << endl;
    m_StdOut << "PitchChange 0 " << channelAkaVoiceGroup << channelAkaGroupToPluckFrequency(channelAkaVoiceGroup) << endl;
}
void MusicFingersSynthesisToolkit::startSynthesizingToStkToolkitStdinFromMusicFingersSerialPort()
{
    connect(this, &MusicFingersSynthesisToolkit::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    CHOOSE_MUSIC_FINGERS_SERIAL_PORT_INFO_OR_DOELSEANDRETURN(chosenSerialPortInfo, emit exitRequested(1))

    MusicFingersSerialPortIntegration *musicFingersSerialPortIntegration = new MusicFingersSerialPortIntegration(chosenSerialPortInfo, this);
    connect(musicFingersSerialPortIntegration, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    if(!musicFingersSerialPortIntegration->open(QIODevice::ReadOnly))
    {
        handleE("failed to open serial port: " + chosenSerialPortInfo.portName() + " (" + chosenSerialPortInfo.description() + " (" + chosenSerialPortInfo.systemLocation() + "))");
        emit exitRequested(1);
        return;
    }
    connect(musicFingersSerialPortIntegration, SIGNAL(fingerMoved(Finger::FingerEnum,int)), this, SLOT(handleFingerMoved(Finger::FingerEnum,int)));


    //set all 10 instruments up and turn them all on :-D
    //m_StdOut << "NoteOn 0 0 64 64" << endl; //Channel 0 doesn't need "ProgramChange" because it is specified as a mandatory cli arg
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(1, 7); //Channel 1, Flute
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(2, 7); //Channel 2, Blown Bottle
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(3, 7); //Channel 3, Bowed String
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(4, 7); //Channel 4, Heavy Metal
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(5, 7); //Channel 5, Percussive Flute
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(6, 7); //Channel 6, B3 Organ
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(7, 7); //Channel 7, FM Voice
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(8, 7); //Channel 8, Simple
    switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(9, 7); //Channel 9, Saxofony (had:Saxophony xD)
}
void MusicFingersSynthesisToolkit::handleO(const QString &msg)
{
    //m_StdOut << msg << endl;
    m_StdErr << msg << endl; //we pipe our output, so...
}
void MusicFingersSynthesisToolkit::handleE(const QString &msg)
{
    m_StdErr << msg;
    m_StdErr.flush();
}
void MusicFingersSynthesisToolkit::handleFingerMoved(Finger::FingerEnum finger, int position)
{
    //hmm, do i want to control stk-demo or do I want to just output skini.... I guess I should choose the latter since it's inherently more modular
    //m_StdOut << "PitchChange 0 " << QString::number(finger) /*TODOoptional: fingerToFingerIndex*/ << " " << position << endl;
    if(position >= 64)
    {
        //NoteOff if on and (plucked at > MIN_PLUCKED_TOGGLE_MS)
        qint64 fingerPluckedAt_OrZeroIfNotPlucked = m_FingersPluckedAt_OrZeroIfNotPlucked.value(finger);
        if(fingerPluckedAt_OrZeroIfNotPlucked == 0) //not on, nothing to do
            return;
        if((QDateTime::currentMSecsSinceEpoch()-fingerPluckedAt_OrZeroIfNotPlucked) > MIN_PLUCKED_TOGGLE_MS)
        {
            //note has been on long enough
            m_FingersPluckedAt_OrZeroIfNotPlucked.insert(finger, 0);
            m_StdOut << "NoteOff 0 " << QString::number(finger) << " " << channelAkaGroupToPluckFrequency((int)finger) << " 127" << endl;
            m_StdOut.flush();
        }
#if 0 //old, not using MIN_PLUCKED_TOGGLE_MS
        if(m_FingersPluckedAt_OrZeroIfNotPlucked.value(finger))
        {
            m_FingersPluckedAt_OrZeroIfNotPlucked.insert(finger, false);
            m_StdOut << "NoteOff 0 " << QString::number(finger) << " " << channelAkaGroupToPluckFrequency((int)finger) << " 127" << endl;
            m_StdOut.flush();
        }
#endif
    }
    else
    {
        //NoteOn if not already on
        qint64 fingerPluckedAt_OrZeroIfNotPlucked = m_FingersPluckedAt_OrZeroIfNotPlucked.value(finger);
        if(fingerPluckedAt_OrZeroIfNotPlucked == 0)
        {
            //not on, turn on
            m_FingersPluckedAt_OrZeroIfNotPlucked.insert(finger, QDateTime::currentMSecsSinceEpoch());
            m_StdOut << "NoteOn 0 " << QString::number(finger) << " " << channelAkaGroupToPluckFrequency((int)finger) << " 64" << endl;
            m_StdOut.flush();
        }
#if 0 //old, not using MIN_PLUCKED_TOGGLE_MS
        if(!m_FingersPluckedAt_OrZeroIfNotPlucked.value(finger))
        {
            m_FingersPluckedAt_OrZeroIfNotPlucked.insert(finger, true);
            m_StdOut << "NoteOn 0 " << QString::number(finger) << " " << channelAkaGroupToPluckFrequency((int)finger) << " 64" << endl;
            m_StdOut.flush();
        }
#endif
    }
}
