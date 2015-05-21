#ifndef CLEANROOMSESSION
#define CLEANROOMSESSION

class CleanRoom;

class CleanRoomSession
{
public:
    CleanRoomSession(CleanRoom *cleanRoom)
        : m_CleanRoom(cleanRoom)
    { }
    CleanRoom *cleanRoom() const
    {
        return m_CleanRoom;
    }
private:
    CleanRoom *m_CleanRoom;
};

#endif // CLEANROOMSESSION

