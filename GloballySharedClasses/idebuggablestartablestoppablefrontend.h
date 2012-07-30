#ifndef IDEBUGGABLESTARTABLESTOPPABLEFRONTEND_H
#define IDEBUGGABLESTARTABLESTOPPABLEFRONTEND_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThread>

class IDebuggableStartableStoppableBackend;

class IDebuggableStartableStoppableFrontend : public QWidget
{
    Q_OBJECT
public:
    explicit IDebuggableStartableStoppableFrontend(IDebuggableStartableStoppableBackend *debuggableStartableStoppableBackend, QWidget *parent = 0);
private:
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartButton;
    QPushButton *m_StopButton;

    QThread *m_BackendThread;
protected:
    IDebuggableStartableStoppableBackend *m_DebuggableStartableStoppableBackend;
    virtual void addButtonsToLayoutAndConnectToBackend()=0;
    QVBoxLayout *m_Layout;
private slots:
    void setupGuiAndThenBeginBackendInit();
    void handleBackendInitialized();
    void handleBackendStarted();
    void handleBackendStopped();
public slots:
    void handleD(const QString &msg);
};

#endif // IDEBUGGABLESTARTABLESTOPPABLEFRONTEND_H
