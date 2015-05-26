#include <Wt/WBreak>
#include <Wt/WLabel>
#include <Wt/WPushButton>
using namespace Wt;
#include "documenttimelineregisterwebdialogwidget.h"

#include "documenttimelinecommon.h"

DocumentTimelineRegisterWebDialogWidget::DocumentTimelineRegisterWebDialogWidget()
    : WDialog(DocumentTimelineWebWidget_REGISTER)
    , m_MessageBox(NULL)
{
    setModal(false);
    setClosable(true);

    WString fullNameString("Full Name");
    WLabel *fullNameLabel = new WLabel(fullNameString + ":", contents());
    m_FullNameLineEdit = new WLineEdit(contents());
    m_FullNameLineEdit->setEmptyText(fullNameString);

    new WBreak(contents());

    WString desiredUsernameString("Desired Username");
    WLabel *desiredUsernameLabel = new WLabel(desiredUsernameString + ":", contents());
    m_DesiredUsernameLineEdit = new WLineEdit(contents());
    m_DesiredUsernameLineEdit->setEmptyText(desiredUsernameString);

    new WBreak(contents());

    WString passwordString("Password");
    WLabel *passwordLabel = new WLabel(passwordString + ":", contents());
    m_PasswordLineEdit = new WLineEdit(contents());
    m_PasswordLineEdit->setEchoMode(WLineEdit::Password);
    m_PasswordLineEdit->setEmptyText(passwordString);

    new WBreak(contents());

    WString confirmPasswordString("Confirm Password");
    WLabel *confirmPasswordLabel = new WLabel(confirmPasswordString + ":", contents());
    m_ConfirmPasswordLineEdit = new WLineEdit(contents());
    m_ConfirmPasswordLineEdit->setEchoMode(WLineEdit::Password);
    m_ConfirmPasswordLineEdit->setEmptyText(confirmPasswordString);

    new WBreak(contents());

    new WLabel("TODOreq: CLA goes here", contents());

    new WBreak(contents());

    m_AcceptedCLACheckBox = new WCheckBox("I Accept", contents());

    new WBreak(contents());

    WString fullNameSiguatureString("Full Name Signature");
    WLabel *fullNameSignatureLabel = new WLabel(fullNameSiguatureString + ":", contents());
    m_FullNameSignatureLineEdit = new WLineEdit(contents());
    m_FullNameSignatureLineEdit->setEmptyText(fullNameSiguatureString);

    new WBreak(contents());

    WPushButton *cancelButton = new WPushButton("Cancel", contents());
    cancelButton->clicked().connect(this, &DocumentTimelineRegisterWebDialogWidget::reject);
    WPushButton *submitButton = new WPushButton("Submit", contents());
    submitButton->clicked().connect(this, &DocumentTimelineRegisterWebDialogWidget::handleSubmitClicked);
}
QString DocumentTimelineRegisterWebDialogWidget::fullName() const
{
    WString fullNameWString = m_FullNameLineEdit->text();
    QString ret = QString::fromUtf8(fullNameWString.toUTF8().c_str());
    return ret;
}
QString DocumentTimelineRegisterWebDialogWidget::desiredUsername() const
{
    WString desiredUsernameWString = m_DesiredUsernameLineEdit->text();
    QString ret = QString::fromUtf8(desiredUsernameWString.toUTF8().c_str());
    return ret;
}
QString DocumentTimelineRegisterWebDialogWidget::password() const
{
    WString passwordWString = m_PasswordLineEdit->text();
    QString ret = QString::fromUtf8(passwordWString.toUTF8().c_str());
    return ret;
}
bool DocumentTimelineRegisterWebDialogWidget::acceptedClaCheckbox() const
{
    return m_AcceptedCLACheckBox->isChecked();
}
QString DocumentTimelineRegisterWebDialogWidget::fullNameSignature() const
{
    WString fullNameSignatureWString = m_FullNameSignatureLineEdit->text();
    QString ret = QString::fromUtf8(fullNameSignatureWString.toUTF8().c_str());
    return ret;
}
DocumentTimelineRegisterWebDialogWidget::~DocumentTimelineRegisterWebDialogWidget()
{
    deleteMessageBoxIfInstantiated();
}
void DocumentTimelineRegisterWebDialogWidget::handleSubmitClicked()
{
    if(registerSubmissionDetailsAreValid())
        accept();
    else
    {
        if(m_MessageBox)
            delete m_MessageBox;
        m_MessageBox = new WMessageBox();
        m_MessageBox->setCaption("Invalid Registration Details");
        m_MessageBox->setText("Some of your registration details were invalid. Please correct them before continuing");
        m_MessageBox->setModal(false);
        m_MessageBox->finished().connect(this, &DocumentTimelineRegisterWebDialogWidget::handleMessageBoxFinished);
        m_MessageBox->show();
    }
}
bool DocumentTimelineRegisterWebDialogWidget::registerSubmissionDetailsAreValid() const
{
    return true; //TODOreq
}
void DocumentTimelineRegisterWebDialogWidget::handleMessageBoxFinished(WDialog::DialogCode dialogCode)
{
    Q_UNUSED(dialogCode)
    deleteMessageBoxIfInstantiated();
}
void DocumentTimelineRegisterWebDialogWidget::deleteMessageBoxIfInstantiated()
{
    if(m_MessageBox)
    {
        delete m_MessageBox;
        m_MessageBox = NULL;
    }
}
