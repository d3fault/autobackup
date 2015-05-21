#ifndef CLEANROOMWIDGET_H
#define CLEANROOMWIDGET_H

#include <QWidget>
#include "icleanroomsession.h"

class CleanRoomGuiSessionResponder;

class CleanRoomWidget : public QWidget, public ICleanRoomSession
{
    Q_OBJECT
public:
    CleanRoomWidget(QWidget *parent = 0);
private:
    void handleRegistrationAttemptFinished(ICleanRoomSessionRequest *request);
private slots:
    void handleRegisterButtonClicked();
    void handleLoginButtonClicked();
    void handleSubmitButtonClicked();
    void handleLogoutButtonClicked();

    //void respond(ICleanRoomSessionResponder *response);
};

#endif // CLEANROOMWIDGET_H
