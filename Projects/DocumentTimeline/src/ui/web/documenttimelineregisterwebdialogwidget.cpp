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

    m_FullNameLineEdit = new WLineEdit(contents());
    m_FullNameLineEdit->setEmptyText("Full Name");

    new WBreak(contents());

    m_DesiredUsernameLineEdit = new WLineEdit(contents());
    m_DesiredUsernameLineEdit->setEmptyText("Desired Username");

    new WBreak(contents());

    m_PasswordLineEdit = new WLineEdit(contents());
    m_PasswordLineEdit->setEchoMode(WLineEdit::Password);
    m_PasswordLineEdit->setEmptyText("Password");

    new WBreak(contents());

    m_ConfirmPasswordLineEdit = new WLineEdit(contents());
    m_PasswordLineEdit->setEchoMode(WLineEdit::Password);
    m_PasswordLineEdit->setEmptyText("Confirm Password");

    new WLabel("TODOreq: CLA text goes here", contents());

    m_AcceptedCLACheckBox = new WCheckBox("I Accept", contents());

    new WBreak(contents());

    m_FullNameSignatureLineEdit = new WLineEdit(contents());
    m_FullNameSignatureLineEdit->setEmptyText("Full Name Signature");

    new WBreak(contents());

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
    }
}
bool DocumentTimelineRegisterWebDialogWidget::registerSubmissionDetailsAreValid() const
{
    return true; //TODOreq
}
void DocumentTimelineRegisterWebDialogWidget::handleMessageBoxFinished(WDialog::DialogCode dialogCode)
{
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
