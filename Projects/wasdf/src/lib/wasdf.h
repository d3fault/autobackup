#ifndef WASDF_H
#define WASDF_H

#include <QObject>

#include <QHash>
#include <QPointer>

#include "ArduinoWriteChangesOnAnalogPinsToSerial.ino" //for common defines only

class WasdfArduino;
class WasdfCalibrator;

enum class Finger
{
    LeftPinky_Finger0 = 0
  , Finger0_LeftPinky = LeftPinky_Finger0

  , LeftRing_Finger1 = 1
  , Finger1_LeftRing = LeftRing_Finger1

  , LeftMiddle_Finger2 = 2
  , Finger2_LeftMiddle = LeftMiddle_Finger2

  , LeftIndex_Finger3 = 3
  , Finger3_LeftIndex = LeftIndex_Finger3

  , LeftThumb_Finger4 = 4
  , Finger4_LeftThumb = LeftThumb_Finger4

  , RightThumb_Finger5 = 5
  , Finger5_RightThumb = RightThumb_Finger5

  , RightIndex_Finger6 = 6
  , Finger6_RightIndex = RightIndex_Finger6

  , RightMiddle_Finger7 = 7
  , Finger7_RightMiddle = RightMiddle_Finger7

  , RightRing_Finger8 = 8
  , Finger8_RightRing = RightRing_Finger8

  , RightPinky_Finger9 = 9
  , Finger9_RightPinky = RightPinky_Finger9
};
QString fingerEnumToHumanReadableString(Finger finger);
inline uint qHash(const Finger &key, uint seed)
{
    return qHash(static_cast<int>(key), seed);
}
struct WasdfCalibrationFingerConfiguration
{
    void calculateAtRestRange(int *out_AtRestMin, int *out_AtRestMax) const;

    int AnalogPinIdOnArduino = -1;

    //min is initialized to max, and max initialized to min, so we can check as we go for values "lower than" or "greater than" etc...
    int MinValue = MaxAnalogPinValue;
    int MaxValue = MinAnalogPinValue;

    int AtRestPosition; //TODOreq: we'd hardcode ~10% range around this AtRestPosition (until the AtRestMaxValue/AtRestMinValue is implemented, at which point percentages (or this mid-point) are no longer used). also worth noting that the range min/max calulcated using percentages must be constrained to 0-1023

    //TODOeventually:
    //int AtRestMinValue;
    //int AtRestMaxValue;
};

class WasdfCalibrationConfiguration : public QHash<Finger, WasdfCalibrationFingerConfiguration>
{
public:
    bool hasFingerWithAnalogPinId(int analogPinId) const;
    Finger getFingerByAnalogPinId(int analogPinId) const;
};

class Wasdf : public QObject
{
    Q_OBJECT
public:
    explicit Wasdf(QObject *parent = 0);
    static bool fingerIsLeftHand(Finger finger);
private:
    void configureArduino();

    WasdfArduino *m_Arduino;
    QPointer<WasdfCalibrator> m_Calibrator;
    WasdfCalibrationConfiguration m_Calibration;
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void fingerMoved(Finger finger, int newPosition);
    void wasdfFinished(bool success);
public slots:
    void startWasdf();
private slots:
    void handleCalibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration);
    void handleAnalogPinReadingChanged(int analogPinId, int newPosition);
};

#endif // WASDF_H
