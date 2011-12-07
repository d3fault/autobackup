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
    void setPendingNodeToAdd(DiagramSceneNode *pendingNode);
    DiagramSceneNode *getPendingNode();
private:
    ModeSingleton();
    static ModeSingleton *m_pInstance;
    DiagramSceneNode *m_PendingNode;
signals:
    void modeChanged(Mode newMode);
public slots:
    void setMode(Mode newMode);

};

#endif // MODESINGLETON_H
