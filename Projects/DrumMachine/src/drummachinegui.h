#ifndef DRUMMACHINEGUI_H
#define DRUMMACHINEGUI_H

#include <QWidget>

class QKeyEvent;

class DrumMachineGui : public QWidget
{
    Q_OBJECT
public:
    DrumMachineGui(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *keyEvent);
};

#endif // DRUMMACHINEGUI_H
