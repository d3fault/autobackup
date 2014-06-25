#ifndef OSIOSGUI_H
#define OSIOSGUI_H

#include "../iosiosui.h"

class OsiosGui : public IOsiosUi
{
    Q_OBJECT
public:
    explicit OsiosGui(QObject *parent = 0);
protected:
    virtual QObject *presentUi();
public slots:
    virtual void handleE(const QString &msg);
};

#endif // OSIOSGUI_H
