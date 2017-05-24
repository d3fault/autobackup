#ifndef WASDFCALIBRATIONATRESTDETECTOR_H
#define WASDFCALIBRATIONATRESTDETECTOR_H

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include "wasdf.h"

class WasdfCalibrationAtRestDetector
{
public:
    void addAnalogPinPosition(int analogPinId, int newPinPosition);
    void weJustFinishedCalibratingThisAnalogPinId(Finger fingerWeJustFinishedCalibrating, int analogPinWeJustFinishedCalibrating);
    void finalize(WasdfCalibrationConfiguration *out_Calibration);
private:
    struct PinNumberAndSensorValue
    {
        PinNumberAndSensorValue(int pinNumber, int sensorValue)
            : PinNumber(pinNumber)
            , SensorValue(sensorValue)
        { }
        int PinNumber;
        int SensorValue;
    };
    typedef QSharedPointer<QList<int/*sensor value*/> > SharedPointerToListOfIntSensorValues;
    typedef QSharedPointer<QList<PinNumberAndSensorValue> > SharedPointerToListOf_PinNumbersAndSensorValues;

    int calculateAverage(const QList<int> &numbersToAverageTogether);
    QHash<int /*pin number on arduino*/, SharedPointerToListOfIntSensorValues> m_CurrentAllAnalogPinPositions_ByAnalogPinId;
    QHash<Finger /*finger that was being calibrated when we gathered these averages*/, SharedPointerToListOf_PinNumbersAndSensorValues> m_AveragedAtRestValuesGatheredWhileCalibratingFingers_ByFinger;
};

#endif // WASDFCALIBRATIONATRESTDETECTOR_H
