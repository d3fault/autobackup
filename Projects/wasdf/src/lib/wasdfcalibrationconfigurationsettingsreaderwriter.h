#ifndef WASDFCALIBRATIONCONFIGURATIONSETTINGSREADERWRITER_H
#define WASDFCALIBRATIONCONFIGURATIONSETTINGSREADERWRITER_H

#include <QSettings>

#include "wasdf.h"

struct WasdfCalibrationConfigurationSettingsReaderWriter
{
    static void writeToSettings(QSettings &settings, const WasdfCalibrationConfiguration &wasdfCalibrationConfiguration);
    static void readFromSettings(QSettings &settings, WasdfCalibrationConfiguration *out_wasdfCalibrationConfiguration);
};


#endif // WASDFCALIBRATIONCONFIGURATIONSETTINGSREADERWRITER_H
