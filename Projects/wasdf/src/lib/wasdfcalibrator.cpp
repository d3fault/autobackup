#include "wasdfcalibrator.h"

#include <QTimer>
#include <QHashIterator>

#include "wasdf.h"

//Calibrator or Calibrater? fuggit~
//need to write this somewhere to remember/understand it better: DURING calibration there's information pertaining to every pin (>= 10) on the arduino, but ONCE CALIBRATED we only want to emit/deliver information pertaining to exactly 10 pins. it's our job as the calibrator to figure out which 10. it is (was) slightly confusing that there are 2 sets of "configutations"
//TODOreq: "at rest" detection/calibration, which should be a RANGE of motion where we ignore. it needn't be hugging the extremeties (0 or 1023), can be right smack in the middle. that range-size should initially be a hardcoded percentage (10%?) of their full 0-1023 utilization, but longer term would be nice if they could specify their own range-size during the calibration phase
//^"at rest" detection could be done after all 10 fingers are calibrated ("ok now put your fingers at rest and keep them still for 5 seconds"), or (ideally) we could do it on the fly while doing the initial 10 finger calibration (the less steps to calibration, the better). to do it "on the fly" requires careful logic: I should detect the "at rest" point (which the at rest 'range' is based around) for a finger on my right hand while a finger on my left hand is being pin-detected-and-full-movement-range-calibrated (aka normal calibration). the reason for this is that fingers on the same hand tend to bend alongside other ones (whereas fingers on the opposite don't). an example flow: while calibrating left index finger, do at rest detection of right index finger; while calibrating right middle finger, do at rest detection of left index finger, so on and so forth. I'm not quite sure when the at rest detection of the very last finger (left pinky) should be done, but there's bound to be sometime to do it eventually~. maybe during the "ok now choose which finger you want as your ok/next/continue/yes finger?" stage (which, if it even exists (right index finger for now)) follows immediately after the calibration stage.
//^^I thought long and hard about it, and finally found a design I like that isn't toooooo complex and also doesn't require special handling of the last finger (left pinky). Basically WHILE we're doing analog pin detection and [normal use] range calibratioin we can be analyzing all of the ANALOG PINS (had:fingers) that aren't currently being calibrated for their "at rest" state. We basically just collect as much data as we can and then make sense of the data after the last finger has been calibrated. It doesn't matter that we don't know which finger each analog pin belongs to, because when we finally do "make sense of the data" we can then match up the analog pins to the fingers (and discard data for analog pins that don't have a finger now mapped). I do still want to only use "at rest" values that were collected while calibrating a fingers on the OPPOSITE hand, so really that's just dropping a couple data sets (once we know which analog pins map to each finger), no problem. Ultimately we'd get (or have remaining) 5 "at rest" values per finger, which we can average together I guess (but hey each of those 5 "at rest" values will themselves probably be averages of the ranges used for the 5 _seconds_ of calibration (happening on the opposite hand))
WasdfCalibrator::WasdfCalibrator(QObject *parent)
    : QObject(parent)
    , m_Timer(new QTimer(this))
{
    m_Timer->setInterval(5000); //TODOmb: visualize the countdown second by second
    m_Timer->setSingleShot(true);
    connect(m_Timer, &QTimer::timeout, this, &WasdfCalibrator::handleTimerTimedOut);
}
WasdfCalibrator::PinNumDetectionAndCalibrationData WasdfCalibrator::getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled()
{
    PinNumDetectionAndCalibrationData currentPinDataWithFurthestAccumulatedDistance;
    QHashIterator<int /*pin number on arduino*/, PinNumDetectionAndCalibrationData> it(m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId);
    while(it.hasNext())
    {
        it.next();
        if(it.value().AccumulatedDistanceThePinHasMoved > currentPinDataWithFurthestAccumulatedDistance.AccumulatedDistanceThePinHasMoved)
        {
            currentPinDataWithFurthestAccumulatedDistance = it.value();
        }
    }
    //TODOreq: error checking? what if all pins didn't move at all for example, would likely mean the arduino is wired wrong
    return currentPinDataWithFurthestAccumulatedDistance;
}
void WasdfCalibrator::calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger()
{
    if(m_OrderedFingerIterator.hasNext())
        calibrateFingerAndThenCallCalibrateNextFingerAgain(m_OrderedFingerIterator.next());
    else
    {
        m_AtRestDetector.finalize(&m_Calibration);
        emit calibrationComplete(m_Calibration);
    }
}
void WasdfCalibrator::calibrateFingerAndThenCallCalibrateNextFingerAgain(Finger fingerToCalibrate)
{
    m_FingerCurrentlyBeingCalibrated = fingerToCalibrate;
    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.clear();
    emit o("Move your " + fingerEnumToHumanReadableString(fingerToCalibrate) + " finger back and forth as much as you can and as far as you can for a few seconds. Try to keep your other fingers as still as possible (but in a position you find comfortable and natural)."); //TODOreq: the wording here sucks, the user will be confused: "aren't as far and as much the same?" an animated .gif explaining it to the user would be best, but even that gif needs to have carefully selected wording xD
    //TODOreq: ^ "left/right thumb finger" sounds dumb kek. there are probably other usages of that dumb-ness ofc. the solution is ez but busy work
    m_Timer->start();
}
void WasdfCalibrator::handleAnalogPinReadingChanged(int pinNumberOnArduino, int newPinPosition)
{
    PinNumDetectionAndCalibrationData currentPinData = m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.value(pinNumberOnArduino, PinNumDetectionAndCalibrationData());

    //this line of code is to facilitate analog pin number detection:
    currentPinData.AccumulatedDistanceThePinHasMoved += static_cast<long>(qAbs(currentPinData.PreviousPinPosition - newPinPosition));

    //these next few lines of code are for finger movement range calibration:
    currentPinData.PinCalibrationData.AnalogPinIdOnArduino = pinNumberOnArduino;
    currentPinData.PinCalibrationData.MaxValue = qMax(currentPinData.PinCalibrationData.MaxValue, newPinPosition);
    currentPinData.PinCalibrationData.MinValue = qMin(currentPinData.PinCalibrationData.MinValue, newPinPosition);

    //this line of code is to facilitate finger "at rest" position calibration
    m_AtRestDetector.addAnalogPinPosition(pinNumberOnArduino, newPinPosition);

    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.insert(pinNumberOnArduino, currentPinData);
}
void WasdfCalibrator::startCalibrating()
{
    m_OrderedFingerIterator.reset(); //not really necessary unless they call startCalibrating again at a later date (RE-calibration)
    calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
}
void WasdfCalibrator::handleTimerTimedOut()
{
    //pseudo: m_FingerCurrentlyBeingCalibrated = m_FingerThatMovedTheMostTotalDistance;

    PinNumDetectionAndCalibrationData pinCalibrationDataOfFingerCurrentlyBeingCalibrated = getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled();

    m_AtRestDetector.weJustFinishedCalibratingThisAnalogPinId(pinCalibrationDataOfFingerCurrentlyBeingCalibrated.PinCalibrationData.AnalogPinIdOnArduino);

    m_Calibration.insert(m_FingerCurrentlyBeingCalibrated, pinCalibrationDataOfFingerCurrentlyBeingCalibrated.PinCalibrationData);

    emit o("We've determined that your " + fingerEnumToHumanReadableString(m_FingerCurrentlyBeingCalibrated) + " finger is connected to arduino's analog pin #" + QString::number(pinCalibrationDataOfFingerCurrentlyBeingCalibrated.PinCalibrationData.AnalogPinIdOnArduino));
    calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
}
