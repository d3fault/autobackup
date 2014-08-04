#ifndef DIRECTMETHODCALLVSVIRTUALMETHODCALLVSINVOKEMETHODVSSIGNALSLOTACTIVATIONCLI_H
#define DIRECTMETHODCALLVSVIRTUALMETHODCALLVSINVOKEMETHODVSSIGNALSLOTACTIVATIONCLI_H

#include <QObject>
#include <QTextStream>

class DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivationCli : public QObject
{
    Q_OBJECT
public:
    explicit DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivationCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
private slots:
    void handleO(const QString &msg);
};

#endif // DIRECTMETHODCALLVSVIRTUALMETHODCALLVSINVOKEMETHODVSSIGNALSLOTACTIVATIONCLI_H
