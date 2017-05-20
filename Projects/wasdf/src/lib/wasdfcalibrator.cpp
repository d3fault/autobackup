#include "wasdfcalibrator.h"

#include <QTimer>
#include <QHashIterator>

#include "wasdf.h"

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
int WasdfCalibrator::getPinNumberWithTheFurthestAccumulatedDistanceTraveled()
{
    int currentPinIdWinning = 0;
    long accumulatedDistanceOfCurrentPinWinning = 0;
    QHashIterator<int /*pin number on arduino*/, PinCalibrationData> it(m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId);
    while(it.hasNext())
    {
        it.next();
        if(it.value().AccumulatedDistanceThePinHasMoved > accumulatedDistanceOfCurrentPinWinning)
        {
            currentPinIdWinning = it.key();
            accumulatedDistanceOfCurrentPinWinning = it.value().AccumulatedDistanceThePinHasMoved;
        }
    }
    //TODOreq: error checking? what if all pins didn't move at all for example, would likely mean the arduino is wired wrong
    return currentPinIdWinning;
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
    PinCalibrationData currentPinData = m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.value(pinNumberOnArduino, PinCalibrationData());
    currentPinData.AccumulatedDistanceThePinHasMoved += static_cast<long>(abs(currentPinData.PreviousPinPosition - newPinPosition));
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

    int pinNumberOfRightIndexFinger = getPinNumberWithTheFurthestAccumulatedDistanceTraveled();
    WasdfCalibrationFingerConfiguration fingerConfiguration = m_Calibration.value(Finger::RightIndex_Finger6, WasdfCalibrationFingerConfiguration());
    fingerConfiguration.AnalogPinIdOnArduino = pinNumberOfRightIndexFinger;
    //TODOreq: we should have min/max values at this point and should assign them to fingerConfiguration
    m_Calibration.insert(Finger::RightIndex_Finger6, fingerConfiguration);
    emit o("We've determined that your right index finger is connected to arduino's analog pin #" + QString::number(pinNumberOfRightIndexFinger));

    //TODreq: eventually:
    emit calibrationComplete(m_Calibration);
}
