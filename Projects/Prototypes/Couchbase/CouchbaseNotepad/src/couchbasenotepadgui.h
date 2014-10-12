#ifndef COUCHBASENOTEPADGUI_H
#define COUCHBASENOTEPADGUI_H

#include <QObject>

class CouchbaseNotepadWidget;

class CouchbaseNotepadGui : public QObject
{
    Q_OBJECT
public:
    explicit CouchbaseNotepadGui(QObject *parent = 0);
    ~CouchbaseNotepadGui();
private:
    CouchbaseNotepadWidget *m_Gui;
public slots:
    void handleCouchbaseNotepadReadyForConnections(QObject *couchbaseNotepadAsQObject);
};

#endif // COUCHBASENOTEPADGUI_H
