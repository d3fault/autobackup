#include "wasdfcalibrator.h"

#include <QTimer>
#include <QHashIterator>

#include "wasdf.h"

//Calibrator or Calibrater? fuggit~
//need to write this somewhere to remember/understand it better: DURING calibration there's information pertaining to every pin (>= 10) on the arduino, but ONCE CALIBRATED we only want to emit/deliver information pertaining to exactly 10 pins. it's our job as the calibrator to figure out which 10. it is (was) slightly confusing that there are 2 sets of "configutations"
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
    PinNumDetectionAndCalibrationData currentPinDataWinning;
    QHashIterator<int /*pin number on arduino*/, PinNumDetectionAndCalibrationData> it(m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId);
    while(it.hasNext())
    {
        it.next();
        if(it.value().AccumulatedDistanceThePinHasMoved > currentPinDataWinning.AccumulatedDistanceThePinHasMoved)
        {
            currentPinDataWinning = it.value();
        }
    }
    //TODOreq: error checking? what if all pins didn't move at all for example, would likely mean the arduino is wired wrong
    return currentPinDataWinning;
}
void WasdfCalibrator::calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger()
{
    if(m_OrderedFingerIterator.hasNext())
        calibrateFingerAndThenCallCalibrateNextFingerAgain(m_OrderedFingerIterator.next());
    else
        emit calibrationComplete(m_Calibration);
}
void WasdfCalibrator::calibrateFingerAndThenCallCalibrateNextFingerAgain(Finger fingerToCalibrate)
{
    m_FingerCurrentlyBeingCalibrated = fingerToCalibrate;
    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.clear();
    emit o("Move your right index finger back and forth as much as you can and as far as you can for a few seconds."); //TODOreq: the wording here sucks, the user will be confused: "aren't as far and as much the same?" an animated .gif explaining it to the user would be best, but even that gif needs to have carefully selected wording xD
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

    PinNumDetectionAndCalibrationData pinCalibrationDataOfRightIndexFinger = getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled();
    m_Calibration.insert(m_FingerCurrentlyBeingCalibrated, pinCalibrationDataOfRightIndexFinger.PinCalibrationData);
    emit o("We've determined that your " + fingerEnumToHumanReadableString(m_FingerCurrentlyBeingCalibrated) + " finger is connected to arduino's analog pin #" + QString::number(pinCalibrationDataOfRightIndexFinger.PinCalibrationData.AnalogPinIdOnArduino));
    calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
}
