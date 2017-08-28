#include "wasdf.h"

#include <QCoreApplication>
#include <QSettings>

#include "wasdfarduino.h"
#include "wasdfcalibrator.h"
#include "wasdfcalibrationconfigurationsettingsreaderwriter.h"

#define SETTINGS_KEY_IS_CALIBRATED "isCalibrated"

//TODOreq: I should always keep this app compiling. At the top of QuickDirtyStaticGitweb (maybe this should go on wasdf.com, but it makes no difference at this point), I should have an image showing the pc <--usb--> arduino <--10x finger lines--> 10x fingers. Then below it links to binaries for "the big 3" operating systems (or can detect OS in js (nah fuck js. ok well the <noscript> would be the big 3 links)). Preferably the binaries are auto-built and auto-deployed. Just like a lot of projects have "nightlies"... mine should be redeployed with every commit/push
//TODOreq: ^that implies I use a GUI instead of a CLI. The overwhelming vast majority of users double click on cli apps and wonder why nothing happened (or maybe they DID notice the terminal flash for a brief second and then disappear, but they don't understand wtf that means). Honestly GUI is only barely more difficult to code anyways (or maybe not even. perhaps just the same amount of difficulty to code (perhaps less)). I do still want to not DEPEND on gui to be able to use this (Wasdf) class (and therefore _all_ functionality of wasdf, hopefully), but yea that's obvious. Also I should make the switch to GUI sooner rather than later (and play catch up with the less used CLI (bbbbut scriptability..... ffffff (oh wait it's a lib ;-P))
Wasdf::Wasdf(QObject *parent)
    : QObject(parent)
    , m_Arduino(new WasdfArduino(this))
{
    QCoreApplication::setOrganizationName("wasdf organization");
    QCoreApplication::setOrganizationDomain("wasdf.com");
    QCoreApplication::setApplicationName("wasdf");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    connect(m_Arduino, &WasdfArduino::e, this, &Wasdf::e);
}
bool Wasdf::fingerIsLeftHand(Finger finger)
{
    switch(finger)
    {
        case Finger::LeftPinky_Finger0:
        case Finger::LeftRing_Finger1:
        case Finger::LeftMiddle_Finger2:
        case Finger::LeftIndex_Finger3:
        case Finger::LeftThumb_Finger4:
            return true;
        break;

        case Finger::RightThumb_Finger5:
        case Finger::RightIndex_Finger6:
        case Finger::RightMiddle_Finger7:
        case Finger::RightRing_Finger8:
        case Finger::RightPinky_Finger9:
            return false;
        break;
    }
    return true; //shouldn't (won't) get here
}
void Wasdf::startWasdfActualSinceCalibrated()
{
    connect(m_Arduino, &WasdfArduino::fingerMoved, this, &Wasdf::handleFingerMoved);
    m_Arduino->start(m_Calibration); //TODOoptimization: once the AtRestPosition or AtRestMinValue/AtRestMinValue stuff is implemented, it would be best if the arduino was told of those values and DIDN'T WRITE TO SERIAL whenever a finger was considered "at rest"... as opposed to us filtering that out on the PC side. the quieter we keep the serial line, the less chance of corruption/errors
}
void Wasdf::startWasdf()
{
    QSettings settings;
    bool isCalibrated = settings.value(SETTINGS_KEY_IS_CALIBRATED, false).toBool();
    if(!isCalibrated)
    {
        m_Arduino->startInCalibrationMode();
        m_Calibrator = new WasdfCalibrator(this);

        //TODOreq: wtf for some reason this doesn't work with qt5-style syntax, which is weird because the WasdfArduino::e signal connects just fine to Wasdf::e signal. I also confirmed that the QPointer isn't the problem
        //connect(m_Calibrator.data(), &WasdfCalibrator::o, this &Wasdf::o);
        connect(m_Calibrator.data(), SIGNAL(o(QString)), this, SIGNAL(o(QString))); //qt4-style connect for now

        connect(m_Arduino, &WasdfArduino::analogPinReadingChangedDuringCalibration, m_Calibrator.data(), &WasdfCalibrator::handleAnalogPinReadingChanged);
        connect(m_Calibrator.data(), &WasdfCalibrator::calibrationComplete, this, &Wasdf::handleCalibrationComplete);
        m_Calibrator->startCalibrating();
    }
    else
    {
        WasdfCalibrationConfigurationSettingsReaderWriter::readFromSettings(settings, &m_Calibration);
        emit o("Calibration read from settings");
        startWasdfActualSinceCalibrated();
    }
}
void Wasdf::handleCalibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration)
{
    m_Calibration = calibrationConfiguration;
    QSettings settings;
    WasdfCalibrationConfigurationSettingsReaderWriter::writeToSettings(settings, calibrationConfiguration);
    if(!m_Calibrator.isNull())
    {
        disconnect(m_Arduino, &WasdfArduino::analogPinReadingChangedDuringCalibration, m_Calibrator.data(), &WasdfCalibrator::handleAnalogPinReadingChanged);
        m_Calibrator->deleteLater();
    }
    startWasdfActualSinceCalibrated();
}
void Wasdf::handleFingerMoved(Finger finger, int newPosition)
{
    //TODOreq: [BEGIN] this is only here for testing
    if(finger != Finger::RightIndex_Finger6)
        return;
    //TODOreq: [END] this is only here for testing


    emit o("Finger '" + fingerEnumToHumanReadableString(finger) + "' moved to position: " + QString::number(newPosition));
    //TODOreq: emit signalForModulesToListenTo(finger, newPosition); //example modules are: MusicCreation, CustomGesturesThatTriggerArbitraryActions /*and by Actions I mean Code*/, and possibly even a KeyboardAndMouse mode if I want to support more Arduino boards. Oh I think there is (at the very least, because long term I definitely want to persist the stream of input (and ideally the "states" of the modules if they can't be fully reproduced using that persisted input stream)) "Keyboard/Mouse" dupe channel mode[le] since idfk how to read the USB keyboard/mouse channel that the Leonardo/micro use. The sketch that calls Mouse.move(to) should be the same sketch that calls Serial.prinln(serializedMouseMoveEvent). They should be only 1 line of code apart and their structures identical (formed one from the other, or at the same time intelligently). This signal should be in the "signals: */private:*/" visibility (non-existent visibility xD (actually you know what there's no bad reason why someone else might want to instantiate Wasdf and then connect to that signal (publicly), so nvm it should NOT be foe-declared-private like this /*private:*/). still I want Wasdf instance to own (be parent of) the modules that the signal I emit are connected to, so yea in that usage of the signal it does make sense that it's "private") of Wasdf


    //TODOreq: [BEGIN] this is only here for testing
    static int numMovementsReceived = 0;
    ++numMovementsReceived;
    if(numMovementsReceived == 2000)
        emit wasdfFinished(true);
    //TODOreq: [END] this is only here for testing
}
QString fingerEnumToHumanReadableString(Finger finger)
{
    //optimized for most commonly used fingers (middle fingers vs thumbs is a tie if you ask me xD. I'd say thumb beats middle for DAILY (normal non-wasdf) USE, but this isn't going to be daily use now is it? ex: when using your mouse you use your middle finger to right click a lot)
    switch(finger)
    {
        case Finger::Finger6_RightIndex:
            return "right index";
        break;
        case Finger::Finger3_LeftIndex:
            return "left index";
        break;
        case Finger::Finger7_RightMiddle:
            return "right middle";
        break;
        case Finger::Finger2_LeftMiddle:
            return "left middle";
        break;
        case Finger::Finger5_RightThumb:
            return "right thumb";
        break;
        case Finger::Finger4_LeftThumb:
            return "left thumb";
        break;
        case Finger::Finger8_RightRing:
            return "right ring";
        break;
        case Finger::Finger1_LeftRing:
            return "left ring";
        break;
        case Finger::Finger9_RightPinky:
            return "right pinky";
        break;
        case Finger::Finger0_LeftPinky:
            return "left pinky"; //rip left pinky, nobody loves/uses him (jk I hope to)
        break;
    }
    return "#error finger#"; //should (will) never get here
}
void WasdfCalibrationFingerConfiguration::calculateAtRestRange(int *out_AtRestMin, int *out_AtRestMax) const
{
#define WASDF_AT_REST_RANGE_PERCENT 10
    //calculate range based on AtRestValue using a hardcdoed percentage _OF_ the calibrated MinValue<-->MaxValue range (not of 0-1023)
    //constrain it to within MinValue<-->MaxValue (inclusive) too
    qreal halfAtRestRangePercent = static_cast<qreal>(WASDF_AT_REST_RANGE_PERCENT) / 2.0; //half for the upper range, half for the lower range
    qreal calibratedTotalRange = qAbs(MaxValue - MinValue);
    int halfAtRestRange = qRound(calibratedTotalRange * (halfAtRestRangePercent / 100.0));
    *out_AtRestMin = qMax(AtRestPosition - halfAtRestRange, MinValue); //get the higher of the 2 values: calculated at rest min vs. calibrated min value
    *out_AtRestMax = qMin(AtRestPosition + halfAtRestRange, MaxValue); //get the lower of the 2 values: calculated at rest max vs. calibrated max value
}
bool WasdfCalibrationConfiguration::hasFingerWithAnalogPinId(int analogPinId) const
{
    QHashIterator<Finger, WasdfCalibrationFingerConfiguration> it(*this);
    while(it.hasNext())
    {
        it.next();
        if(it.value().AnalogPinIdOnArduino == analogPinId)
            return true;
    }
    return false;
}
Finger WasdfCalibrationConfiguration::getFingerByAnalogPinId(int analogPinId) const
{
    //NOTE: a finger with that analog pin id MUST exist or the program crashes xD. use hasFingerWithAnalogPinId() first

    QHashIterator<Finger, WasdfCalibrationFingerConfiguration> it(*this);
    while(it.hasNext())
    {
        it.next();
        if(it.value().AnalogPinIdOnArduino == analogPinId)
            return it.key();
    }
    qFatal("WasdfCalibrationConfiguration::getFingerByAnalogPinId called with invalid pinId. make sure it exists first by calling WasdfCalibrationConfiguration::hasFingerWithAnalogPinId");
    return Finger::Finger0_LeftPinky; //won't get here
}
