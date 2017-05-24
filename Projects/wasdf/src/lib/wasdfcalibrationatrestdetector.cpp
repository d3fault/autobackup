#include "wasdfcalibrationatrestdetector.h"

#include <QHashIterator>
#include <QMutableHashIterator>

//TODOmb: like I'm pushing this functionality into it's own class, so too should I (probably/maybe, but also maybe not [yet] if it's too much work to refactor xD) push the "analog pin detection" and "normal use range detection" into their own classes. Additionally, all calibration related classes should go in a ./calibration subfolder. The WasdfCalibrator should simply hasA these 3 types of calibration 'mode' thingies (for lack of a better word)
void WasdfCalibrationAtRestDetector::addAnalogPinPosition(int analogPinId, int newPinPosition)
{
    m_CurrentAllAnalogPinPositions_ByAnalogPinId.value(analogPinId, QSharedPointer<QList<int> >(new QList<int /*sensor value*/>()))->append(newPinPosition);
}
void WasdfCalibrationAtRestDetector::weJustFinishedCalibratingThisAnalogPinId(int analogPinIdToDiscardCurrentPinPositionsFor)
{
    //0) discard the positions for the analog pin id passed in (because we told the user to move it a bunch, so it wasn't "at rest" at all!)
    m_CurrentAllAnalogPinPositions_ByAnalogPinId.remove(analogPinIdToDiscardCurrentPinPositionsFor);

    //now discard all position data for fingers on the same hand <---- NVM. although we know the finger that was just calibrated, we have no idea (yet) what finger the rest of them are (ok that's only partially true. as we get nearer and nearer to the last finger we know more and more fingers. still, simpler to discard "same hand" values at the very end in finalize, when we know ALL fingers)

    //1) average all the remaining at rest positions gathered in the last 5 seconds
    QHashIterator<int /*pin number on arduino*/, QSharedPointer<QList<int /*sensor value*/> > > it(m_CurrentAllAnalogPinPositions_ByAnalogPinId);
    while(it.hasNext())
    {
        it.next();
        int average = calculateAverage(*(it.value().data()));
        m_RunningAllAnalogPinsAtRestPositionsToBeAveragedYetAgainAtTheVeryEndOfCalibration_ByAnalogPinId.value(it.key(), QSharedPointer<QList<int> >(new QList<int>()))->append(average);
    }

    //2) clear the 'current' lists for the next 5 seconds of calibration (assuming there will be more, but it doesn't matter if there isn't)
    m_CurrentAllAnalogPinPositions_ByAnalogPinId.clear();
}
void WasdfCalibrationAtRestDetector::finalize(WasdfCalibrationConfiguration *out_Calibration)
{
    //first discard all position data collected for fingers on the same hand
    //fuck, 'staggered' deletion of averages? fuck, my head hurts, what? like uhh when Finger6_RightIndex was being calibrated we delete the index 0 at rest average of the right hand, then Figner5_LeftIndex was being calibrated we delete the index 0 at rest average of the left hand, then Finger7_RightMiddle we delete index 1.. WAIT NO if we 'removed' index 0 (during RightIndex), then the new one we remove is going to be index 0 again!!! not 'staggered' after all. still my head hurts, typing this out certainly helps. so I use my ordered finger iterator I think. hmm I need to map the analog pins to the fingers first though, wait no that's already done in out_Calibration. wait no it IS staggered after all, since index 0 [now] points to an at rest value that we want to keep (one gathered while the opposite hand finger was being calibrated)! fuck, my head!!! still that just means a ++indexToDeleteOnRightHand and a ++indexToDeleteOnLeftHand, or something? hmmmm to make things easier for my head I could record which finger each set of "at rest" values corresponds to. Hmmmm QHash<Finger, QSharedPointer<QList> > atRestValues_ByFingerBeingCalibratedWhenTheyWereDetected ??? then I iterate that hash, map the analog pins to the fingers, determine if it's the same hand as in that QHash's key, drop that reading if it is, if it's opposite then add it to a newly built QHash<Finger, QSharedPointer<QList> >, then finally after that iteration of droppage, iterate the newly built one simply to calculate the average rest value and THEN stuff it into out_Calibration
    //TODOreq: ^

    //get all the at rest readings for the fingers connected to analog pins we actually care about, then average those all together one last time, then stuff that final "at rest" value into the calibration
    QMutableHashIterator<Finger, WasdfCalibrationFingerConfiguration> it(*out_Calibration);
    while(it.hasNext())
    {
        it.next();
        WasdfCalibrationFingerConfiguration fingerConfiguration = it.value();
        int analogPinId = fingerConfiguration.AnalogPinIdOnArduino;
        QSharedPointer<QList<int> > atRestReadings = m_RunningAllAnalogPinsAtRestPositionsToBeAveragedYetAgainAtTheVeryEndOfCalibration_ByAnalogPinId.value(analogPinId);
        int finalAveragedAtRestReading = calculateAverage(*(atRestReadings.data()));
        fingerConfiguration.AtRestPosition = finalAveragedAtRestReading;
        it.setValue(fingerConfiguration);
    }
}
int WasdfCalibrationAtRestDetector::calculateAverage(const QList<int> &numbersToAverageTogether)
{
    double result = 0.0;
    Q_FOREACH(int currentNumber, numbersToAverageTogether)
    {
        result += currentNumber;
    }
    result /= static_cast<double>(numbersToAverageTogether.size());
    //TODOoptional: round, don't truncate
    return static_cast<int>(result);
}
