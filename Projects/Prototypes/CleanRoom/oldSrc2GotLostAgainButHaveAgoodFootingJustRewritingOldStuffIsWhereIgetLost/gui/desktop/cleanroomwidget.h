#ifndef CLEANROOMWIDGET_H
#define CLEANROOMWIDGET_H

#include <QWidget>
//#include "cleanroomsession.h"

#include <QSharedPointer>

class CleanRoomSession;
class CleanRoomGuiResponder;

class QLineEdit;

class CleanRoomWidget : public QWidget//, public CleanRoomSession
{
    Q_OBJECT
public:
    CleanRoomWidget(QWidget *parent = 0);
    ICleanRoomFrontEndResponder *responder() const;
private:
    CleanRoomSession *m_Session;
    CleanRoomGuiResponder *m_Responder;

    QLineEdit *m_RegisterUsernameLineEdit;
    QLineEdit *m_RegisterPasswordLineEdit;

    void handleRegistrationAttemptFinished(ICleanRoomSessionRequest *request);
//signals:
//    void sessionRequestRequested(ICleanRoomSessionRequest *request);
private slots:
    void handleRegisterButtonClicked();
    void handleLoginButtonClicked();
    void handleSubmitButtonClicked();
    void handleLogoutButtonClicked();

    //void respond(ICleanRoomFrontEndResponder *response);

public slots:
    void handleCleanRoomSessionStarted(CleanRoomSession *session);
    void cleanRoomApiCallback(ICleanRoomSessionRequest *request);

    void handleFrontPageDefaultViewReceived(QList<QString> frontPageDocs);
};

#endif // CLEANROOMWIDGET_H
