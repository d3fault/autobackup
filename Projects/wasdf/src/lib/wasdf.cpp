#include "wasdf.h"

#include <QCoreApplication>
#include <QSettings>

#include "wasdfcalibrator.h"

#define SETTINGS_KEY_IS_CALIBRATED "isCalibrated"

Wasdf::Wasdf(QObject *parent)
    : QObject(parent)
    , m_Calibrator(0)
{
    QCoreApplication::setOrganizationName("wasdf organization");
    QCoreApplication::setOrganizationDomain("wasdf.com");
    QCoreApplication::setApplicationName("wasdf");
}
void Wasdf::startWasdfActualSinceCalibrated()
{

}
void Wasdf::startWasdf()
{
    m_Arduino.startCommunicatingOverSerialPort();
    QSettings settings;
    bool isCalibrated = settings.value(SETTINGS_KEY_IS_CALIBRATED, false).toBool();
    if(!isCalibrated)
    {
        m_Calibrator = new WasdfCalibrator(this);
        connect(m_Calibrator.data(), SIGNAL(calibrationComplete(WasdfCalibrationConfiguration)), this, SLOT(handleCalibrationComplete(WasdfCalibrationConfiguration)));
        QMetaObject::invokeMethod(m_Calibrator.data(), "startCalibrating");
    }
    else
    {
        //TODOreq: read WasdfCalibrationConfiguration out of the settings (or maybe just read values on-demand when needed)
        qDebug("Calibrated!");
        startWasdfActualSinceCalibrated();
    }
}
void Wasdf::handleCalibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration)
{
    //TODoreq: write calibrationConfiguration to settings
    if(!m_Calibrator.isNull())
        m_Calibrator->deleteLater();
    startWasdfActualSinceCalibrated();
}
