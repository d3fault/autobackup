#ifndef RECURSIVEFILESTRINGREPLACERENAMERGUI_H
#define RECURSIVEFILESTRINGREPLACERENAMERGUI_H

#include <QObject>
#include <QCoreApplication>

#include "../lib/recursivefilestringreplacerenamer.h"
#include "recursivefilestringreplacerenamerwidget.h"
#include "objectonthreadhelper.h"

class RecursiveFileStringReplaceRenamerGui : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveFileStringReplaceRenamerGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<RecursiveFileStringReplaceRenamer> m_BusinessThread;
    RecursiveFileStringReplaceRenamerWidget m_Gui;
public slots:
    void handleRecursiveFileStringReplaceRenamerReadyForConnections();
    void handleAboutToQuit();
};

#endif // RECURSIVEFILESTRINGREPLACERENAMERGUI_H
