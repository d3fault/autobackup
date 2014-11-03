#include "osiosdhtbootstrapsplashdialog.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QCheckBox>

#include "osiossettings.h"

//QDialog because QSplashScreen doesn't allow user input :(
//The CLI equivalent should just block until bootstrap is complete, duh
//TODOoptional: this would also change the above comment to be untrue, as now simply blocking the in CLI equivalent is not good enough: allow adding bootstrap nodes at runtime in this dialog!
//TODOoptional: debug mode only maybe: copy well-formed cli invocation including my own, and all healthy, IP/ports... to clipboard (speeds up testing for me)
OsiosDhtBootstrapSplashDialog::OsiosDhtBootstrapSplashDialog(const QString &lastUsedProfileNameToDisplayInSplash, bool autoLoginLastUsedProfileOnBootstrap, QWidget *parent)
    : QDialog(parent)
{
    //TODOoptional: pretty-ify, animate the status messages as they come, etc

    setWindowTitle(OSIOS_HUMAN_READABLE_TITLE);

    QVBoxLayout *myLayout = new QVBoxLayout(this);

    m_DhtMessagesTextEdit = new QTextEdit();
    m_DhtMessagesTextEdit->setReadOnly(true);

    QCheckBox *autoLoginCheckbox = new QCheckBox(tr("Auto-Login as '") + lastUsedProfileNameToDisplayInSplash + tr("' when DHT finishes bootstrapping") /* connected, meaning bootstrapped.. but the user doesn't care*/);
    autoLoginCheckbox->setChecked(autoLoginLastUsedProfileOnBootstrap && !lastUsedProfileNameToDisplayInSplash.isEmpty());
    autoLoginCheckbox->setEnabled(!lastUsedProfileNameToDisplayInSplash.isEmpty());

    myLayout->addWidget(m_DhtMessagesTextEdit);
    myLayout->addWidget(autoLoginCheckbox);

    //TODOreq: cancel/quit, should be another signal on our interface!
}
QObject *OsiosDhtBootstrapSplashDialog::asQObject()
{
    return this;
}
void OsiosDhtBootstrapSplashDialog::presentNotification(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel)
{
    Q_UNUSED(notificationLevel)
    m_DhtMessagesTextEdit->append(notificationMessage); //TODOoptional: colorize like in normal status notification panel, however these messages are much less important than there so colorizing isn't a huge deal
}
void OsiosDhtBootstrapSplashDialog::handleBootstrapped()
{
    accept(); //TODOreq: use auto-login checkbox contents in next screen (profile manager unless first launch). TODOreq: specifying profile takes precedence over last used profile, and also checks the (both) auto-login checkboxes (they can still be opted out of like normal)
}
