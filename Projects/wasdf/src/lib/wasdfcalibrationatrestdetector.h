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
    int calculateAverage(const QList<int> &numbersToAverageTogether);
    QHash<int /*pin number on arduino*/, QSharedPointer<QList<int/*sensor value*/> > > m_CurrentAllAnalogPinPositions_ByAnalogPinId;
    QHash<Finger /*finger that was being calibrated when we gathered these averages*/, QSharedPointer<QList<QPair<int /*pin number on arduino*/, int /*sensor value*/> > > > m_AveragedAtRestValuesGatheredWhileCalibratingFingers_ByFinger;
};

#endif // WASDFCALIBRATIONATRESTDETECTOR_H
