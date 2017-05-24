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
    struct PinNumberAndAtRestValue
    {
        PinNumberAndAtRestValue(int pinNumber, int atRestValue)
            : PinNumber(pinNumber)
            , AtRestValue(atRestValue)
        { }
        int PinNumber;
        int AtRestValue;
    };
    typedef QSharedPointer<QList<int/*at rest value*/> > SharedPointerToListOfIntAtRestValues;
    typedef QSharedPointer<QList<PinNumberAndAtRestValue> > SharedPointerToListOf_PinNumbersAndAtRestValues;

    int calculateAverage(const QList<int> &numbersToAverageTogether);
    QHash<int /*pin number on arduino*/, SharedPointerToListOfIntAtRestValues> m_CurrentAllAnalogPinPositions_ByAnalogPinId;
    QHash<Finger /*finger that was being calibrated when we gathered these averages*/, SharedPointerToListOf_PinNumbersAndAtRestValues> m_AveragedAtRestValuesGatheredWhileCalibratingFingers_ByFinger;
};

#endif // WASDFCALIBRATIONATRESTDETECTOR_H
