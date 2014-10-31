#include "osiosnotificationswidget.h"

#include <QVBoxLayout>
#include <QTabWidget>
#include <QTextEdit>
#include <QDateTime>
#include "osiossettings.h"

//TODOoptional: instead of tabs i could do just one text edit and provide checkboxes for what levels to show. Perhaps I should not allow the ignoring/hiding of error and fatal messages
//TODOmb: give better default height. stupidly my main window is giving it about half the height :-/
OsiosNotificationsWidget::OsiosNotificationsWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->setContentsMargins(OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS);

    QTabWidget *differentNotiticationLevelsTabWidget = new QTabWidget();
    differentNotiticationLevelsTabWidget->setTabPosition(QTabWidget::South);

    m_AllNotificationLevelsTextEdit = myMakeTextEdit();
    m_StandardNotificationLevelTextEdit = myMakeTextEdit();
    m_WarningNotificationLevelTextEdit = myMakeTextEdit();
    m_ErrorNotificationLevelTextEdit = myMakeTextEdit();
    m_FatalNotificationLevelTextEdit = myMakeTextEdit();

    differentNotiticationLevelsTabWidget->addTab(m_AllNotificationLevelsTextEdit, tr("All Notifications"));
    differentNotiticationLevelsTabWidget->addTab(m_StandardNotificationLevelTextEdit, tr("Standard Notifications"));
    differentNotiticationLevelsTabWidget->addTab(m_WarningNotificationLevelTextEdit, tr("Warning Notifications"));
    differentNotiticationLevelsTabWidget->addTab(m_ErrorNotificationLevelTextEdit, tr("Error Notifications"));
    differentNotiticationLevelsTabWidget->addTab(m_FatalNotificationLevelTextEdit, tr("Fatal Notifications"));

    myLayout->addWidget(differentNotiticationLevelsTabWidget);

    setLayout(myLayout);
}
QTextEdit *OsiosNotificationsWidget::myMakeTextEdit()
{
    QTextEdit *ret = new QTextEdit();
    ret->setReadOnly(true);
    return ret;
}
QString OsiosNotificationsWidget::eyeGrabbingErrorNotification(const QString &notificationMessage)
{
    return QString("<i><b><font color='red'>" + notificationMessage + "</font></b></i>");
}
void OsiosNotificationsWidget::presentNotification(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel)
{
    QString notificationMessagePlain = QDateTime::currentDateTime().toString(Qt::RFC2822Date) + " - ";
    QString notificationMessageHtml;
    switch(notificationLevel)
    {
        case OsiosNotificationLevels::StandardNotificationLevel:
        {
            notificationMessagePlain += "Info: " + notificationMessage;
            notificationMessageHtml = notificationMessagePlain;
            m_StandardNotificationLevelTextEdit->append(notificationMessagePlain);
        }
            break;
        case OsiosNotificationLevels::WarningNotificationLevel:
        {
            notificationMessagePlain += "Warning: " + notificationMessage;
            notificationMessageHtml = "<i>" + notificationMessagePlain + "</i>";
            m_WarningNotificationLevelTextEdit->append(notificationMessagePlain);
        }
            break;
        case OsiosNotificationLevels::ErrorNotificationLevel:
        {
            notificationMessagePlain += "Error: " + notificationMessage;
            notificationMessageHtml = eyeGrabbingErrorNotification(notificationMessagePlain);
            m_ErrorNotificationLevelTextEdit->append(notificationMessagePlain); //TODOreq: indicate the tab needs attention
        }
            break;
        case OsiosNotificationLevels::FatalNotificationLevel:
        default:
        {
            notificationMessagePlain += "Fatal: " + notificationMessage;
            notificationMessageHtml = eyeGrabbingErrorNotification(notificationMessagePlain);
            m_FatalNotificationLevelTextEdit->append(notificationMessagePlain); //TODOreq: indicate the tab needs attention
        }
            break;
    }
    qDebug() << notificationMessagePlain;
    m_AllNotificationLevelsTextEdit->append(notificationMessageHtml);
}
