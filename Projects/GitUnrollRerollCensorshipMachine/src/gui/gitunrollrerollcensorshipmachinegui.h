#ifndef GITUNROLLREROLLCENSORSHIPMACHINEGUI_H
#define GITUNROLLREROLLCENSORSHIPMACHINEGUI_H

#include <QObject>
#include <QCoreApplication>

#include "gitunrollrerollcensorshipmachine.h"
#include "gitunrollrerollcensorshipmachinewidget.h"
#include "objectonthreadhelper.h"

class GitUnrollRerollCensorshipMachineGui : public QObject
{
    Q_OBJECT
public:
    explicit GitUnrollRerollCensorshipMachineGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<GitUnrollRerollCensorshipMachine> m_BusinessThread;
    GitUnrollRerollCensorshipMachineWidget m_Gui;
public slots:
    void handleBusinessReadyForConnections();
    void handleAboutToQuit();
};

#endif // GITUNROLLREROLLCENSORSHIPMACHINEGUI_H
