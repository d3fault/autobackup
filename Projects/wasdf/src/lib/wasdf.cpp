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
    connect(m_Arduino, &WasdfArduino::fingerMoved, this, &Wasdf::handleFingerMoved);
    m_Arduino->start(m_Calibration);
}
void Wasdf::startWasdf()
{
    QSettings settings;
    bool isCalibrated = settings.value(SETTINGS_KEY_IS_CALIBRATED, false).toBool();
    if(!isCalibrated)
    {
        m_Arduino->startInCalibrationMode();
        m_Calibrator = new WasdfCalibrator(this);
        connect(m_Arduino, &WasdfArduino::analogPinReadingChangedDuringCalibration, m_Calibrator.data(), &WasdfCalibrator::handleAnalogPinReadingChanged);
        connect(m_Calibrator.data(), &WasdfCalibrator::calibrationComplete, this, &Wasdf::handleCalibrationComplete);
        m_Calibrator->startCalibrating();
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
    m_Calibration = calibrationConfiguration;
    //TODoreq: write calibrationConfiguration to settings
    if(!m_Calibrator.isNull())
    {
        disconnect(m_Arduino, &WasdfArduino::analogPinReadingChangedDuringCalibration, m_Calibrator.data(), &WasdfCalibrator::handleAnalogPinReadingChanged);
        m_Calibrator->deleteLater();
    }
    startWasdfActualSinceCalibrated();
}
void Wasdf::handleFingerMoved(Finger finger, int newPosition)
{
    //TODOreq: it's business time
    qDebug("I have stripped down to my business socks");
}
