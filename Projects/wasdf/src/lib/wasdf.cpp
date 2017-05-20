#include "wasdf.h"

#include <QCoreApplication>
#include <QSettings>

#include "wasdfarduino.h"
#include "wasdfcalibrator.h"

#define SETTINGS_KEY_IS_CALIBRATED "isCalibrated"

Wasdf::Wasdf(QObject *parent)
    : QObject(parent)
    , m_Arduino(new WasdfArduino(this))
{
    QCoreApplication::setOrganizationName("wasdf organization");
    QCoreApplication::setOrganizationDomain("wasdf.com");
    QCoreApplication::setApplicationName("wasdf");
}
void Wasdf::startWasdfActualSinceCalibrated()
{
#ifdef TEMP_DSFLJDS
    m_Arduino.setMode(Normal, m_CalibrationConfig); //TODOreq: this is where the 10 fingers get mapped to the 10 pins
#endif
}
void Wasdf::startWasdf()
{
    //m_Arduino.startCommunicatingOverSerialPort();
    QSettings settings;
    bool isCalibrated = settings.value(SETTINGS_KEY_IS_CALIBRATED, false).toBool();
    if(!isCalibrated)
    {
#ifdef TEMP_DSFLJDS
        m_Arduino.setMode(Calibrating); //TODOreq: arduino sends _ALL_ (not just 10) analog pin readings over serial
#endif
        m_Calibrator = new WasdfCalibrator(this);
        connect(m_Calibrator.data(), &WasdfCalibrator::calibrationComplete, this, &Wasdf::handleCalibrationComplete);
        QMetaObject::invokeMethod(m_Calibrator.data(), "startCalibrating");
    }
    else
    {
        //TODOreq: read WasdfCalibrationConfiguration out of the settings (or maybe just read values on-demand when needed)
        //m_WasdfCalibrationConfiguration = ;
        qDebug("Calibrated!");
        startWasdfActualSinceCalibrated();
    }
}
void Wasdf::handleCalibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration)
{
    //m_WasdfCalibrationConfiguration = calibrationConfiguration;
    //TODoreq: write calibrationConfiguration to settings
    if(!m_Calibrator.isNull())
        m_Calibrator->deleteLater();
    startWasdfActualSinceCalibrated();
}
