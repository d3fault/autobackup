#ifndef SIMPLIFIEDHEIRARCHYMOLESTERWIDGET_H
#define SIMPLIFIEDHEIRARCHYMOLESTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>

#include "labellineeditbrowsebutton.h"

class SimplifiedHeirarchyMolesterWidget : public QWidget
{
    Q_OBJECT
public:
    SimplifiedHeirarchyMolesterWidget(QWidget *parent = 0);
    ~SimplifiedHeirarchyMolesterWidget();
private:
    LabelLineEditBrowseButton *m_DirectoryToMolestRow;
    LabelLineEditBrowseButton *m_LastModifiedTimestampsFileRow;
    QPlainTextEdit *m_Debug;
signals:
    void heirarchyMolestationRequested(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath);
public slots:
    void handleD(const QString &msg);
private slots:
    void handleStartButtonClicked();
};

#endif // SIMPLIFIEDHEIRARCHYMOLESTERWIDGET_H
