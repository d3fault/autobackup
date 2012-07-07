#ifndef DEBUGSTARTSTOPGUI_H
#define DEBUGSTARTSTOPGUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThread>

#include "idebuggablestartablestoppablebackend.h"

class DebugStartStopGui : public QWidget
{
    Q_OBJECT
public:
    explicit DebugStartStopGui(IDebuggableStartableStoppableBackend *debuggableStartableStoppableBackend);
private:
    QThread *m_BackendThread;
    IDebuggableStartableStoppableBackend *m_DebuggableStartableStoppableBackend;

    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartButton;
    QPushButton *m_StopButton;
public slots:
    void handleD(const QString &);
};

#endif // DEBUGSTARTSTOPGUI_H
