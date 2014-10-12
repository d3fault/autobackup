#ifndef COUCHBASENOTEPADGUI_H
#define COUCHBASENOTEPADGUI_H

#include <QObject>

class ObjectOnThreadGroup;
class CouchbaseNotepadWidget;

class CouchbaseNotepadGui : public QObject
{
    Q_OBJECT
public:
    explicit CouchbaseNotepadGui(QObject *parent = 0);
    ~CouchbaseNotepadGui();
private:
    ObjectOnThreadGroup *m_BackendThread;
    CouchbaseNotepadWidget *m_Gui;
signals:
    void quitBackendRequested();
public slots:
    void handleCouchbaseNotepadReadyForConnections(QObject *couchbaseNotepadAsQObject);
private slots:
    void handleAboutToQuit();
};

#endif // COUCHBASENOTEPADGUI_H
