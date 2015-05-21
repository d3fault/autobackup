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
    CleanRoom *m_CleanRoom;
    QScopedPointer<CleanRoomWidget> m_Gui;
private slots:
    void handleCleanRoomReadyForSessions();
    //void handleCleanRoomFinishedGettingFrontPageDefaultView(QList<QString> frontPageDocs);
};

#endif // CLEANROOMGUI_H
