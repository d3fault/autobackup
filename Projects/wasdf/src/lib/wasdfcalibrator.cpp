#include "wasdfcalibrator.h"

WasdfCalibrator::WasdfCalibrator(QObject *parent)
    : QObject(parent)
    , m_FingerCurrentlyBeingCalibrated(Finger0)
{ }
void WasdfCalibrator::calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger()
{
    if(m_CurrentFingerBeingCalibrated == Finger::Finger9)
        emit calibrationComplete(m_Calibration);
    else
    {
        ++m_FingerCurrentlyBeingCalibrated;

    }
}
void WasdfCalibrator::startCalibrating()
{
    calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
}
