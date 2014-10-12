#ifndef COUCHBASENOTEPADWIDGET_H
#define COUCHBASENOTEPADWIDGET_H

#include <QWidget>

class QLineEdit;
class QPlainTextEdit;

class CouchbaseNotepadWidget : public QWidget
{
    Q_OBJECT
public:
    CouchbaseNotepadWidget(QWidget *parent = 0);
private:
    QLineEdit *m_KeyLineEdit;
    QPlainTextEdit *m_Value;
signals:
    void getCouchbaseNotepadDocByKeyRequested(const QString &key);
    void addCouchbaseNotepadDocByKeyRequested(const QString &key, const QString &value); //TODOreq: "overwrite?" dialog, yes = store
public slots:
    void handleE(const QString &msg);

    void handleInitializedAndStartedSuccessfully();

    void handleGetCouchbaseNotepadDocFinished(bool dbError, bool lcbOpSuccess, const QString &key, const QString &value);
    void handleAddCouchbaseNotepadDocFinished(bool dbError, bool lcbOpSuccess, const QString &key, const QString &value); //should i give them the value they just added? guess it depends on CAS stuff tbh, they might want to verify it's the same value etc

    void handleCouchbaseNotepadExitted(bool exittedCleanly);
private slots:
    void handleGetButtonClicked();
    void handleAddButtonClicked();
};

#endif // COUCHBASENOTEPADWIDGET_H
