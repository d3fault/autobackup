#ifndef SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSECLI_H
#define SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSECLI_H

#include <QObject>
#include <QTextStream>

class SimpleAsyncLibrarySlotInvokeAndSignalResponseCli : public QObject
{
    Q_OBJECT
public:
    explicit SimpleAsyncLibrarySlotInvokeAndSignalResponseCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
private slots:
    void handleFooSignal(bool success);
};

#endif // SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSECLI_H
