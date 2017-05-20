#ifndef WASDFCALIBRATIONCONFIGURATION_H
#define WASDFCALIBRATIONCONFIGURATION_H

#include <QHash>

#include "wasdf.h"

struct WasdfCalibrationConfigurationFinger
{
    int MinValue = 1023;
    int MaxValue = 0;
};

struct WasdfCalibrationConfiguration
{
    WasdfCalibrationConfiguration()
    {
        for(Finger finger = Finger0_LeftPinky; finger <= Finger9_RightPinky; ++finger)
        {
            FingerCalibrations.insert(finger, WasdfCalibrationConfigurationFinger());
        }
    }
    QHash<Finger, WasdfCalibrationConfigurationFinger> FingerCalibrations;
};

#endif // WASDFCALIBRATIONCONFIGURATION_H
