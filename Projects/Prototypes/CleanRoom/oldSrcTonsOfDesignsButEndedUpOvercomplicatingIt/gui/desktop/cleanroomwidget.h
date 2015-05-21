#ifndef CLEANROOMWIDGET_H
#define CLEANROOMWIDGET_H

#include <QWidget>
#include "icleanroomsession.h"

#include "cleanroomdoc.h"

class QVBoxLayout;

class CleanRoomWidget : public QWidget, public ICleanRoomSession
{
    Q_OBJECT
public:
    CleanRoomWidget(QWidget *parent = 0);
protected:
    void respond(ICleanRoomSessionRequest *request, ICleanRoomSessionResponse *response);
private:
    QVBoxLayout *m_Layout;
signals:
    void getAndSubscribeToFrontPageRequested(ICleanRoomSession cleanRoomSession);
private slots:
    void handleFrontPageUpdated(ICleanRoomSessionResponse *response);
};

#endif // CLEANROOMWIDGET_H
