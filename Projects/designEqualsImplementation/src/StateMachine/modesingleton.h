#ifndef MODESINGLETON_H
#define MODESINGLETON_H

#include <QObject>
#include "../ClassesThatRepresentProjectBeingCreated/designprojecttemplates.h"

class ModeSingleton : public QObject
{
    Q_OBJECT
public:
    static ModeSingleton* Instance();
    //re: modes. i had to make a choice between graphicsscene being able to see into mainwindow, or mainmenu to see into graphicsscene. i opted for the later because a front end can/is/should-be dependent on the backend... but a backend should never be dependent on a frontend (even though they are both front-ends, this front-end is more back-end than mainwindow
    //modes
    enum Mode { ClickDragDefaultMode, AddNodeMode }; //i loved writing addnodemode
    Mode m_Mode;
    Mode getMode();
private:
    ModeSingleton();
    static ModeSingleton *m_pInstance;
signals:
    //the fix for the signal not being received was to put ModeSingleton:: in front of Mode below. qmake compares the signals/slots "literally" (string value i guess)
    void modeChanged(ModeSingleton::Mode newMode);
public slots:
    void setMode(ModeSingleton::Mode newMode);

};

#endif // MODESINGLETON_H
