#ifndef WASDFCALIBRATOR_H
#define WASDFCALIBRATOR_H

#include <QObject>

#include <QHash>

#include "wasdf.h"

class QTimer;

class WasdfCalibrator : public QObject
{
    Q_OBJECT
public:
    explicit WasdfCalibrator(QObject *parent = 0);
private:
    struct PinCalibrationData
    {
        int PreviousPinPosition = 0;
        long AccumulatedDistanceThePinHasMoved = 0;
    };

    //void calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
    int getPinNumberWithTheFurthestAccumulatedDistanceTraveled();

    QHash<int /*pin number on arduino*/, PinCalibrationData> m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId;
    QTimer *m_Timer;
    //Finger m_FingerCurrentlyBeingCalibrated;
    WasdfCalibrationConfiguration m_Calibration;
signals:
    void o(const QString &msg);
    void calibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration);
public slots:
    void handleAnalogPinReadingChanged(int pinNumberOnArduino, int newPinPosition);
    void startCalibrating();
private slots:
    void handleTimerTimedOut();
};

#endif // WASDFCALIBRATOR_H
