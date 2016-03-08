#ifndef QTSYSTEMSIGNALHANDLERCLI_H
#define QTSYSTEMSIGNALHANDLERCLI_H

#include <QObject>

class QtSystemSignalHandlerCli : public QObject
{
    Q_OBJECT
public:
    explicit QtSystemSignalHandlerCli(QObject *parent = 0);
private slots:
    void handleSigTermOrSigIntReceived();
};

#endif // QTSYSTEMSIGNALHANDLERCLI_H
