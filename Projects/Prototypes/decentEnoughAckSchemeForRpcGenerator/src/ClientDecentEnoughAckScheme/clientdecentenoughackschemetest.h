#ifndef CLIENTDECENTENOUGHACKSCHEMETEST_H
#define CLIENTDECENTENOUGHACKSCHEMETEST_H

#include <QObject>

#include "objectonthreadhelper.h"

#include "clientmainwidget.h"
#include "clientdecentenoughackschemebackend.h"

class ClientDecentEnoughAckSchemeTest : public QObject
{
    Q_OBJECT
public:
    explicit ClientDecentEnoughAckSchemeTest(QObject *parent = 0);
private:
    ClientMainWidget m_Gui;
    ObjectOnThreadHelper<ClientDecentEnoughAckSchemeBackend> m_Backend;
signals:
    
public slots:
    void handleBackendReadyForConnections();
};

#endif // CLIENTDECENTENOUGHACKSCHEMETEST_H
