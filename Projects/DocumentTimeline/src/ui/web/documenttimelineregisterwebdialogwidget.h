#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WCheckBox>
#include <Wt/WMessageBox>
using namespace Wt;
#ifndef DOCUMENTTIMELINEREGISTERWEBDIALOGWIDGET_H
#define DOCUMENTTIMELINEREGISTERWEBDIALOGWIDGET_H

#include <QString>
#include <QScopedPointer>

class DocumentTimelineRegisterWebDialogWidget : public WDialog
{
public:
    DocumentTimelineRegisterWebDialogWidget();
    QString fullName() const;
    QString desiredUsername() const;
    QString password() const;
    bool acceptedClaCheckbox() const;
    QString fullNameSignature() const;
private:
    WLineEdit *m_FullNameLineEdit;
    WLineEdit *m_DesiredUsernameLineEdit;
    WLineEdit *m_PasswordLineEdit;
    WLineEdit *m_ConfirmPasswordLineEdit;
    WCheckBox *m_AcceptedCLACheckBox;
    WLineEdit *m_FullNameSignatureLineEdit;

    QScopedPointer<WMessageBox> m_MessageBox;

    void handleSubmitClicked();
    bool registerSubmissionDetailsAreValid() const;
    void handleMessageBoxFinished(DialogCode dialogCode);
};

#endif // DOCUMENTTIMELINEREGISTERWEBDIALOGWIDGET_H
