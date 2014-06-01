#ifndef LASTMODIFIEDTIMESTAMPSTOOLSGUI_H
#define LASTMODIFIEDTIMESTAMPSTOOLSGUI_H

#include <QCoreApplication>
#include <QObject>

#include "objectonthreadhelper.h"
#include "../lib/lastmodifiedtimestampstools.h"
#include "lastmodifiedtimestampstoolswidget.h"

class LastModifiedTimestampsToolsGui : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsToolsGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<LastModifiedTimestampsTools> m_BusinessThread;
    LastModifiedTimestampsToolsWidget m_Gui;
public slots:
    void handleSimplifiedHeirarchyMolesterReadyForConnections();
    void handleAboutToQuit();
};

#endif // LASTMODIFIEDTIMESTAMPSTOOLSGUI_H
