#ifndef LASTMODIFIEDDATEHEIRARCHYMOLESTERGUI_H
#define LASTMODIFIEDDATEHEIRARCHYMOLESTERGUI_H

#include <QCoreApplication>
#include <QObject>

#include "objectonthreadhelper.h"
#include "lastmodifieddateheirarchymolester.h"
#include "lastmodifieddateheirarchymolesterwidget.h"

class LastModifiedDateHeirarchyMolesterGui : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedDateHeirarchyMolesterGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<LastModifiedDateHeirarchyMolester> m_BusinessThread;
    LastModifiedDateHeirarchyMolesterWidget m_Gui;
public slots:
    void handleLastModifiedDateHeirarchyMolesterReadyForConnections();
    void handleAboutToQuit();
};

#endif // LASTMODIFIEDDATEHEIRARCHYMOLESTERGUI_H
