#ifndef RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESSGUI_H
#define RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESSGUI_H

#include <QObject>

#include "runtimedynamiciterationsdeterminerretainingresponsivenesswidget.h"
#include "objectonthreadhelper.h"
#include "../lib/runtimedynamiciterationsdeterminerretainingresponsiveness.h"

class RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui : public QObject
{
    Q_OBJECT
public:
    explicit RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui(QObject *parent = 0);
private:
    RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget m_Gui;
    ObjectOnThreadHelper<RuntimeDynamicIterationsDeterminerRetainingResponsiveness> m_BusinessThread;
private slots:
    void handleRuntimeDynamicIterationsDeterminerRetainingResponsivenessReadyForConnections();
    void handleAboutToQuit();
};

#endif // RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESSGUI_H
