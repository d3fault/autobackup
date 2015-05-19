#ifndef CLEANROOMWWW_H
#define CLEANROOMWWW_H

#include <QObject>

class CleanRoom;

class CleanRoomWWW : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomWWW(QObject *parent = 0);
private:
    CleanRoom *m_CleanRoom;
};

#endif // CLEANROOMWWW_H
