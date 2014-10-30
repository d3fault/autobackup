#ifndef IBOOTSTRAPSPLASHSCREEN_H
#define IBOOTSTRAPSPLASHSCREEN_H

#include <QUrl>

#include "osioscommon.h"

class QObject;

//could be a QDialog or a CLI [async] prompt
class IOsiosDhtBootstrapSplashScreen
{
public:
    virtual QObject *asQObject()=0;
protected: //signals
    virtual void addBootstrapNodeRequested(const QUrl &newBootstrapNode)=0;
public: //slots
    virtual void presentNotification(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)=0;
    virtual void handleBootstrapped()=0;
};

#endif // IBOOTSTRAPSPLASHSCREEN_H
