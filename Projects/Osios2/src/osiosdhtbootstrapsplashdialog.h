#ifndef OSIOSDHTBOOTSTRAPSPLASHDIALOG_H
#define OSIOSDHTBOOTSTRAPSPLASHDIALOG_H

#include <QDialog>
#include "ibootstrapsplashscreen.h"

class QTextEdit;

class OsiosDhtBootstrapSplashDialog : public QDialog, public IOsiosDhtBootstrapSplashScreen
{
    Q_OBJECT
public:
    explicit OsiosDhtBootstrapSplashDialog(const QString &lastUsedProfileNameToDisplayInSplash, bool autoLoginLastUsedProfileOnBootstrap, QWidget *parent = 0);
    QObject *asQObject();
private:
    QTextEdit *m_DhtMessagesTextEdit;
signals:
    void addBootstrapNodeRequested(const QUrl &newBootstrapNode); //TODOreq
public slots:
    void presentNotification(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel);
    void handleBootstrapped();
};

#endif // OSIOSDHTBOOTSTRAPSPLASHDIALOG_H
