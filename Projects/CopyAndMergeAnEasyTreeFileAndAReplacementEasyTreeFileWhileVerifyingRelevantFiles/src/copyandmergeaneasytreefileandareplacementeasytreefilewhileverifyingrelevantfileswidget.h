#ifndef COPYANDMERGEANEASYTREEFILEANDAREPLACEMENTEASYTREEFILEWHILEVERIFYINGRELEVANTFILESWIDGET_H
#define COPYANDMERGEANEASYTREEFILEANDAREPLACEMENTEASYTREEFILEWHILEVERIFYINGRELEVANTFILESWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFileDialog>

class CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget : public QWidget
{
    Q_OBJECT
public:
    CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget(QWidget *parent = 0);
private:
    QLineEdit *m_RegularEasyTreeFileLineEdit;
    QLineEdit *m_ReplacementEasyTreeFileLineEdit;
    QLineEdit *m_SourceBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLineEdit;
    QLineEdit *m_DestinationDirToCopyToLineEdit;
    QLineEdit *m_OutputEasyTreeFileLineEdit;
    QPlainTextEdit *m_Debug;
signals:
    void copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesRequested(const QString &regularEasyTreeFile, const QString &replacementFormattedEasyTreeFile, const QString &sourceBaseDirCorrespondingToEasyTreeFile, const QString &destinationDirToCopyTo, const QString &outputEasyTreeFile);
private slots:
    void handleCopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesButtonClicked();
    void handleRegularEasyTreeFileBrowseButtonClicked();
    void handleReplacementsEasyTreeFileBrowseButtonClicked();
    void handleInputBaseDirCorrespondingToRegularEasyTreeFilesEasyTreeFileBrowseButtonClicked();
    void handleOutputDirToCopyToBrowseButtonClicked();
    void handleOutputEasyTreeFileBrowseButtonClicked();
public slots:
    void handleD(const QString &msg);
};

#endif // COPYANDMERGEANEASYTREEFILEANDAREPLACEMENTEASYTREEFILEWHILEVERIFYINGRELEVANTFILESWIDGET_H
