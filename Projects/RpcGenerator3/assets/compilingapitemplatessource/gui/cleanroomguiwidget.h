#ifndef CLEANROOMGUIWIDGET_H
#define CLEANROOMGUIWIDGET_H

#include <QWidget>
#include <QScopedPointer>

#include "cleanroomsession.h"

class QLabel;

class ICleanRoom;

class CleanRoomGuiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CleanRoomGuiWidget(ICleanRoom *cleanRoom, QWidget *parent = 0);
private:
    QScopedPointer<CleanRoomSession> m_Session;
    QList<QLabel*> m_CleanRoomDocsWidgets;
public slots:
    void handleNewSessionCreated(CleanRoomSession session);
    void handleFrontPageDefaultViewReceived(QStringList frontPageDocs);
};

#endif // CLEANROOMGUIWIDGET_H
