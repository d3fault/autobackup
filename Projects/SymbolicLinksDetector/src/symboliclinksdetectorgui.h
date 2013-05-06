#ifndef SYMBOLICLINKSDETECTORGUI_H
#define SYMBOLICLINKSDETECTORGUI_H

#include <QObject>
#include <QCoreApplication>

#include "mainwidget.h"
#include "symboliclinksdetectorbusiness.h"
#include "objectonthreadhelper.h"

class SymbolicLinksDetectorGui : public QObject
{
    Q_OBJECT
public:
    explicit SymbolicLinksDetectorGui(QObject *parent = 0);
private:
    MainWidget m_Gui;
    ObjectOnThreadHelper<SymbolicLinksDetectorBusiness> m_Business;
private slots:
    void handleSymbolicLinksDetectorBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // SYMBOLICLINKSDETECTORGUI_H
