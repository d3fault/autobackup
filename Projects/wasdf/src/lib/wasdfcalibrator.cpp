#include "wasdfcalibrator.h"

#include <QTimer>
#include <QHashIterator>

#include "wasdf.h"

//Calibrator or Calibrater? fuggit~
//need to write this somewhere to remember/understand it better: DURING calibration there's information pertaining to every pin (>= 10) on the arduino, but AFTER calibration we only want to emit/deliver information pertaining to exactly 10 pins. it's our job as the calibrator to figure out which 10. it is (was) slightly confusing that there are 2 sets of "configutations"
WasdfCalibrator::WasdfCalibrator(QObject *parent)
    : QObject(parent)
    , m_Timer(new QTimer(this))
    //, m_FingerCurrentlyBeingCalibrated(Finger0)
{
    m_Timer->setInterval(3000);
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
#if 0
void WasdfCalibrator::calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger()
{
    if(m_CurrentFingerBeingCalibrated == Finger::Finger9)
        emit calibrationComplete(m_Calibration);
    else
    {
        ++m_FingerCurrentlyBeingCalibrated;

    }
}
#endif
void WasdfCalibrator::handleAnalogPinReadingChanged(int pinNumberOnArduino, int newPinPosition)
{
    PinNumDetectionAndCalibrationData currentPinData = m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.value(pinNumberOnArduino, PinNumDetectionAndCalibrationData());

    //this line of code is to facilitate analog pin number detection:
    currentPinData.AccumulatedDistanceThePinHasMoved += static_cast<long>(abs(currentPinData.PreviousPinPosition - newPinPosition));

    //these next few lines of code are for finger movement range calibration:
    currentPinData.PinCalibrationData.AnalogPinIdOnArduino = pinNumberOnArduino;
    currentPinData.PinCalibrationData.MaxValue = qMax(currentPinData.PinCalibrationData.MaxValue, newPinPosition);
    currentPinData.PinCalibrationData.MinValue = qMin(currentPinData.PinCalibrationData.MinValue, newPinPosition);

    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.insert(pinNumberOnArduino, currentPinData);
}
void WasdfCalibrator::startCalibrating()
{
    //calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
    emit o("Move your right index finger back and forth as far as much as you can and as far as you can for a few seconds."); //TODOreq: the wording here sucks, the user will be confused: "aren't as far and as much the same?"
    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.clear();
    m_Timer->start();
}
void WasdfCalibrator::handleTimerTimedOut()
{
    //pseudo: RightIndexFinger = m_FingerThatMovedTheMostTotalDistance;

    PinNumDetectionAndCalibrationData pinCalibrationDataOfRightIndexFinger = getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled();
    m_Calibration.insert(Finger::RightIndex_Finger6, pinCalibrationDataOfRightIndexFinger.PinCalibrationData);
    emit o("We've determined that your right index finger is connected to arduino's analog pin #" + QString::number(pinCalibrationDataOfRightIndexFinger.PinCalibrationData.AnalogPinIdOnArduino));

    //TODreq: eventually:
    emit calibrationComplete(m_Calibration);
}
