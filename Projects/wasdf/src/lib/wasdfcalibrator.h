#ifndef WASDFCALIBRATOR_H
#define WASDFCALIBRATOR_H

#include <QObject>

#include "wasdf.h"
#include "wasdfcalibrationconfiguration.h"

class WasdfCalibrator : public QObject
{
    Q_OBJECT
public:
    explicit WasdfCalibrator(QObject *parent = 0);
private:
    void calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();

    Finger m_FingerCurrentlyBeingCalibrated;
    WasdfCalibrationConfiguration m_Calibration;
signals:
    void calibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration);
public slots:
    void startCalibrating();
};

#endif // WASDFCALIBRATOR_H
