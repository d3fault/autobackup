#ifndef BANKSERVERDEBUGWIDGET_H
#define BANKSERVERDEBUGWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThread>

#include "idebuggablestartablestoppablebankserverbackend.h"

class bankServerDebugWidget : public QWidget

{
    Q_OBJECT

public:
    bankServerDebugWidget(IDebuggableStartableStoppableBankServerBackend *debuggableStartableStoppableBankServerBackend);
    ~bankServerDebugWidget();
private:
    QThread *m_BackendThread;
    IDebuggableStartableStoppableBankServerBackend* m_DebuggableStartableStoppableBankServerBackend;
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartButton;
    QPushButton *m_StopButton;

    void handleD(const QString &msg);
private slots:
    void buildGui();
    void handleBackendInitialized();
    void handleBackendStarted();
    void handleBackendStopped();
};

#endif // BANKSERVERDEBUGWIDGET_H
