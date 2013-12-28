#ifndef RECURSIVEFILESTRINGREPLACERENAMERWIDGET_H
#define RECURSIVEFILESTRINGREPLACERENAMERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include "labellineeditbrowsebutton.h"

class RecursiveFileStringReplaceRenamerWidget : public QWidget
{
    Q_OBJECT
public:
    RecursiveFileStringReplaceRenamerWidget(QWidget *parent = 0);
    ~RecursiveFileStringReplaceRenamerWidget();
private:
    LabelLineEditBrowseButton *m_Directory;
    QLineEdit *m_TextToReplaceLineEdit;
    QLineEdit *m_ReplacementTextLineEdit;
    QPlainTextEdit *m_Debug;
signals:
    void recursivelyRenameFilesDoingSimpleStringReplaceRequested(const QString &rootDirectory, const QString &textToReplace, const QString &replacementText);
public slots:
    void handleD(const QString &msg);
private slots:
    void handleStartButtonClicked();
};

#endif // RECURSIVEFILESTRINGREPLACERENAMERWIDGET_H
