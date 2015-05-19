#include "cleanroomwww.h"

#include "cleanroom.h"

CleanRoomWWW::CleanRoomWWW(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{ }
