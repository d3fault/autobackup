#ifndef CLEANROOMGUI_H
#define CLEANROOMGUI_H

#include <QObject>

#include <QScopedPointer>

#include "cleanroomwidget.h"

class CleanRoomGui : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomGui(QObject *parent = 0);
private:
    QScopedPointer<CleanRoomWidget> m_Gui;
private slots:
    void handleCleanRoomReadyForSessions();
};

#endif // CLEANROOMGUI_H
