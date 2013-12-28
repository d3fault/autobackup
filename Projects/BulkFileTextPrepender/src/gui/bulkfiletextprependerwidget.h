#ifndef BULKFILETEXTPREPENDERWIDGET_H
#define BULKFILETEXTPREPENDERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLine>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStringList>

#include "labellineeditbrowsebutton.h"

class BulkFileTextPrependerWidget : public QWidget
{
    Q_OBJECT
public:
    BulkFileTextPrependerWidget(QWidget *parent = 0);
    ~BulkFileTextPrependerWidget();
private:
    LabelLineEditBrowseButton *m_DirSelectRow;
    LabelLineEditBrowseButton *m_FileWithTextToPrepend;
    QLineEdit *m_FilenameExtensionsLineEdit;
    QPlainTextEdit *m_Debug;
signals:
    void prependStringToBeginningOfAllTextFilesInDirRequested(const QString &dir, const QString &textToPrepend, const QStringList &filenameExtensionsToPrependTo);
public slots:
    void handleD(const QString &msg);
private slots:
    void handleStartButtonClicked();
};

#endif // BULKFILETEXTPREPENDERWIDGET_H
