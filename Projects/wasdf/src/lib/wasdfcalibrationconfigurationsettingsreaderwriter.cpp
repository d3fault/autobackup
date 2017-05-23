#include "wasdfcalibrationconfigurationsettingsreaderwriter.h"

#include <QHashIterator>

#define WasdfCalibrationConfigurationSettingsReaderWriter_FINGERS_ARRAY "fingers"
#define WasdfCalibrationConfigurationSettingsReaderWriter_FINGER_KEY "finger"
#define WasdfCalibrationConfigurationSettingsReaderWriter_KEY_ANALOG_PIN_ON_ARDUINO "analogPin"
#define WasdfCalibrationConfigurationSettingsReaderWriter_KEY_MINVALUE "minValue"
#define WasdfCalibrationConfigurationSettingsReaderWriter_KEY_MAXVALUE "maxValue"

//TODOreq: "profiles"
void WasdfCalibrationConfigurationSettingsReaderWriter::writeToSettings(QSettings &settings, const WasdfCalibrationConfiguration &wasdfCalibrationConfiguration)
{
    settings.beginWriteArray(WasdfCalibrationConfigurationSettingsReaderWriter_FINGERS_ARRAY);
    int i = 0;
    QHashIterator<Finger, WasdfCalibrationFingerConfiguration> it(wasdfCalibrationConfiguration);
    while(it.hasNext())
    {
        settings.setArrayIndex(i);
        it.next();

        settings.setValue(WasdfCalibrationConfigurationSettingsReaderWriter_FINGER_KEY, /*fingerEnumToHumanReadableString*/static_cast<int>(it.key())); //I thought (while laying in bed last night) about maybe using this int-converted value in place of "i" for the settings array, but then I'd have to convert the hash to a map before inserting (not hard at all!) and idk it just might lead to problems if for example a "FingerNegative1_InvalidFinger" ever gets introduced. it's also why I changed various static_casts to int instead of uint. you never know what design changes you'll make in the future
        settings.setValue(WasdfCalibrationConfigurationSettingsReaderWriter_KEY_ANALOG_PIN_ON_ARDUINO, it.value().AnalogPinIdOnArduino);
        settings.setValue(WasdfCalibrationConfigurationSettingsReaderWriter_KEY_MINVALUE, it.value().MinValue);
        settings.setValue(WasdfCalibrationConfigurationSettingsReaderWriter_KEY_MAXVALUE, it.value().MaxValue);
        settings.endGroup();

        ++i;
    }
    settings.endArray();
}
void WasdfCalibrationConfigurationSettingsReaderWriter::readFromSettings(QSettings &settings, WasdfCalibrationConfiguration *out_wasdfCalibrationConfiguration) //TO DOnereq: I think settings passed in should be a _const_ reference, but my compiler is saying that settings.beginReadArray (keyword _READ_) discards const qualifiers yada yada. I should see if that's still the case in newer versions of Qt, and file a bug if it is (but really this is not very important). hmm maybe it can't be const because settings has to do cross-process synchronization. yea that's probably it xD
{
    int numFingers = settings.beginReadArray(WasdfCalibrationConfigurationSettingsReaderWriter_FINGERS_ARRAY);
    //TODOmb: verify i == 10 ... however I like this class being dumb/static, so that error checking should be the responsibility of the caller

    for(int i = 0; i < numFingers; ++i)
    {
        settings.setArrayIndex(i);

        Finger finger = static_cast<Finger>(settings.value(WasdfCalibrationConfigurationSettingsReaderWriter_FINGER_KEY).toInt(/*TODOmb: &convertOk error checking that converted to int was successful. also applies to the next few lines of code*/));
        WasdfCalibrationFingerConfiguration fingerConfigutation;
        fingerConfigutation.AnalogPinIdOnArduino = settings.value(WasdfCalibrationConfigurationSettingsReaderWriter_KEY_ANALOG_PIN_ON_ARDUINO).toInt();
        fingerConfigutation.MinValue = settings.value(WasdfCalibrationConfigurationSettingsReaderWriter_KEY_MINVALUE).toInt();
        fingerConfigutation.MaxValue = settings.value(WasdfCalibrationConfigurationSettingsReaderWriter_KEY_MAXVALUE).toInt();

        *out_wasdfCalibrationConfiguration->insert(finger, fingerConfigutation);
    }

    settings.endArray();
}
