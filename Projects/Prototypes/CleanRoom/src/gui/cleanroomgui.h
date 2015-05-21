#ifndef CLEANROOMGUI_H
#define CLEANROOMGUI_H

#include <QObject>
#include <QScopedPointer>

#include "cleanroomguiwidget.h"

class CleanRoom;

class CleanRoomGui : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomGui(QObject *parent = 0);
private:
    CleanRoom *m_CleanRoom;
    QScopedPointer<CleanRoomGuiWidget> m_Gui;
private slots:
    void handleCleanRoomReadyForSessions();
};

#endif // CLEANROOMGUI_H
