#ifndef SIMPLIFIEDHEIRARCHYMOLESTERGUI_H
#define SIMPLIFIEDHEIRARCHYMOLESTERGUI_H

#include <QCoreApplication>
#include <QObject>

#include "objectonthreadhelper.h"
#include "simplifiedheirarchymolester.h"
#include "simplifiedheirarchymolesterwidget.h"

class SimplifiedHeirarchyMolesterGui : public QObject
{
    Q_OBJECT
public:
    explicit SimplifiedHeirarchyMolesterGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<SimplifiedHeirarchyMolester> m_BusinessThread;
    SimplifiedHeirarchyMolesterWidget m_Gui;
public slots:
    void handleSimplifiedHeirarchyMolesterReadyForConnections();
    void handleAboutToQuit();
};

#endif // SIMPLIFIEDHEIRARCHYMOLESTERGUI_H
