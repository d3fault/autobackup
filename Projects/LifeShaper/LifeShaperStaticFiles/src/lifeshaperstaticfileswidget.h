#ifndef LIFESHAPERSTATICFILESWIDGET_H
#define LIFESHAPERSTATICFILESWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QCheckBox>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QFont>

class LifeShaperStaticFilesWidget : public QWidget
{
    Q_OBJECT    
public:
    LifeShaperStaticFilesWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;

    QLineEdit *m_InputEasyTreeFileLineEdit;
    QLineEdit *m_InputDirThatEasyTreeFileSpecifiesLineEdit;
    QString m_InputDirThatEasyTreeFileSpecifies; //with a slash definitely added
    QLineEdit *m_OutputFilesDirLineEdit;
    QString m_OutputFilesFolder; //with a a slash definitely added
    QLineEdit *m_LineLeftOffOnLastRunLineEdit;

    QLineEdit *m_CurrentEasyTreeHashLineLineEdit;

    QPushButton *m_StartIteratingEasyTreeHashFile;
    QPushButton *m_StopIteratingEasyTreeHashFile;

    //Analysis
    QLabel *m_RelativeFilePathLabel;
    QPushButton *m_CopyAbsolutePathToClipboard;

    //Attributes
    QLabel *m_IsDirLabel;
    QLabel *m_FileSizeLabel;
    QLabel *m_CreationDateTimeLabel;
    QLabel *m_LastModifiedDateTimeLabel;


    QCheckBox *m_ApplyToThisAndAllFollowingUntilParentDirReEntered; //recursion checkbox
    //Decision Buttons:
    QPushButton *m_LeaveBehindButton;
    QPushButton *m_IffyCopyrightButton;
    QPushButton *m_DeferDecicionButton;
    QPushButton *m_BringForwardButton;
    QPushButton *m_Use_THIS_AsReplacementButton;
    QLineEdit *m_FilePathForTHISreplacement; //TODOreq: make sure we still use the old attribs (where applicable -- timestamps and filename mainly)
    QPushButton *m_Use_THIS_AsReplacementBrowseButton;

    void setGuiEnabledBasedOnWhetherStarted(bool started);
    void setDecisionsButtansEnabled(bool enabled);

    QString getFullPathOfCurrentItem();
    bool m_CurrentItemIsDir;
signals:
    void startIteratingEasyTreeHashFileRequested(QString easyTreeHashFile, QString outputFilesFolder, QString lineLeftOffFrom);
    void stopIteratingRequested();
    void recursionFromCurrentChanged(bool);
    void leaveBehindDecided();
    void iffyCopyrightDecided();
    void deferDecisionDecided(); //lol
    void bringForwardDecided();
    void useTHISasReplacementDecided(QString filePathForTHISreplacement);
public slots:
    void handleInputEasyTreeFileBrowseButtonClicked();
    void handleInputDirThatEasyTreeFileSpecifiesBrowseButtonClicked();
    void handleOutputFilesFolderBrowseButtonClicked();

    void handleStartIteratingEasyTreeHashFileButtonClicked();
    void handleStopIteratingEasyTreeHashFileButtonClicked();

    void handleCopyAbsolutePathToClipboardClicked();
    void handleOpenWithMousepadClicked();
    void handleLaunchWithDefaultApplicationClicked();
    //void handleOpenDirectoryInTerminalClicked();
    void handleOpenInWebBrowserlClicked();
    void handleOpenInMplayerClicked();

    void handleNowProcessingEasyTreeHashItem(QString easyTreeHashLine, QString relativeFilePath, bool isDirectory, qint64 fileSize_OR_zeroIfDir, QString creationDateTime, QString lastModifiedDateTime);

    void handleStoppedRecursingDatOperationBecauseParentDirEntered();

    void handleBrowseForReplacementButtonClicked();
    void handleUseTHISasReplacementClicked();
    void aDecisionButtonWasClicked();
    void handleFinishedProcessingEasyTreeHashFile();
    void handleD(const QString &msg);
};

#endif // LIFESHAPERSTATICFILESWIDGET_H
