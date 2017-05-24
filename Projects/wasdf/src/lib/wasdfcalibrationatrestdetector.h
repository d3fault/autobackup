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
    void weJustFinishedCalibratingThisAnalogPinId(int analogPinIdToDiscardCurrentPinPositionsFor);
    void finalize(WasdfCalibrationConfiguration *out_Calibration);
private:
    int calculateAverage(const QList<int> &numbersToAverageTogether);
    QHash<int /*pin number on arduino*/, QSharedPointer<QList<int/*sensor value*/> > > m_CurrentAllAnalogPinPositions_ByAnalogPinId;
    QHash<int /*pin number on arduino*/, QSharedPointer<QList<int/*sensor value*/> > > m_RunningAllAnalogPinsAtRestPositionsToBeAveragedYetAgainAtTheVeryEndOfCalibration_ByAnalogPinId;
};

#endif // WASDFCALIBRATIONATRESTDETECTOR_H
