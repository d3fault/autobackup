#ifndef OSIOSNOTIFICATIONSWIDGET_H
#define OSIOSNOTIFICATIONSWIDGET_H

#include <QWidget>

#include "osioscommon.h"

class QTextEdit;

class OsiosNotificationsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OsiosNotificationsWidget(QWidget *parent = 0);
private:
    QTextEdit *m_AllNotificationLevelsTextEdit;
    QTextEdit *m_StandardNotificationLevelTextEdit;
    QTextEdit *m_WarningNotificationLevelTextEdit;
    QTextEdit *m_ErrorNotificationLevelTextEdit;
    QTextEdit *m_FatalNotificationLevelTextEdit;

    static QTextEdit *myMakeTextEdit();
    QString eyeGrabbingErrorNotification(const QString &notificationMessage);
public slots:
    void presentNotification(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel);
};

#endif // OSIOSNOTIFICATIONSWIDGET_H
