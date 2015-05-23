#ifndef CLEANROOMGUI_H
#define CLEANROOMGUI_H

#include <QObject>
#include <QScopedPointer>

#include "cleanroomguiwidget.h"

class ICleanRoom;

class CleanRoomGui : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomGui(QObject *parent = 0);
private:
    ICleanRoom *m_CleanRoom;
    QScopedPointer<CleanRoomGuiWidget> m_Gui;
signals:
    void initializeAndStartCleanRoomRequested();
private slots:
    void handleCleanRoomReadyForSessions();
};

#endif // CLEANROOMGUI_H
