#ifndef SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEGUI_H
#define SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEGUI_H

#include <QObject>

class SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget;

class SimpleAsyncLibrarySlotInvokeAndSignalResponseGui : public QObject
{
    Q_OBJECT
public:
    explicit SimpleAsyncLibrarySlotInvokeAndSignalResponseGui(QObject *parent = 0);
    ~SimpleAsyncLibrarySlotInvokeAndSignalResponseGui();
private:
    SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget *m_Gui;
private slots:
    void handleFooReadyForConnections(QObject *fooAsQObject);
};

#endif // SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEGUI_H
