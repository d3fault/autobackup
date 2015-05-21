#ifndef CLEANROOMGUI_H
#define CLEANROOMGUI_H

#include <QObject>

class CleanRoom;

class CleanRoomGui : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomGui(QObject *parent = 0);
private:
    CleanRoom *m_CleanRoom;
};

#endif // CLEANROOMGUI_H
