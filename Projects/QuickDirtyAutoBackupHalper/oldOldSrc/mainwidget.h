#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QThread>
#include <QDateTime>
#include <QProcess>

#include "quickdirtyautobackuphalper.h"

#define AUTO_COMMIT_DATE_REPLACE_SPECIAL_TXT "%AUTOCOMMITDATE%"

class mainWidget : public QWidget
{
    Q_OBJECT
    
public:
    mainWidget(QuickDirtyAutoBackupHalper *business, QWidget *parent = 0);
    ~mainWidget();
private:
    QuickDirtyAutoBackupHalper *m_Business;
    QThread *m_BusinessThread;

    bool m_Mounted;
    bool m_AboutToCommitWaitingForMount;
    bool m_Committing;
    bool m_AboutToShutdownWaitingForDismount;

    void probeMountStatus();
    void updateGuiBasedOnMountedOrDismounted();
    void continueCommitProcessBecauseMounted();
    void continueShuttingDownBecauseDismounted();
    void rigConnections();

    //GUI
    void buildGui();
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QCheckBox *m_MountAsReadOnly;
    QPushButton *m_MountTcVolumesButton;
    QPushButton *m_DismountTcVolumesButton;
    QLineEdit *m_GitCommitMessage;
    QPushButton *m_MountIfNeededAndCommitButton; //TODOreq: error out early if "read only" is checked
    QPushButton *m_ShutdownAfterDismountButton;
private slots:
    void initialize();
    void handleBusinessInitializedAndProbed(bool fucked, bool theyAreMounted);
    void handleMountBtnClicked();
    void handleMounted();
    void handleDismounted();
    void handleCommitBtnClicked();
    void handleCommitComplete();
    void handleShutdownBtnClicked();
public slots:
    void handleD(const QString &msg);
signals:
    void mountRequested(bool mountAsReadOnly);
    void commitRequested(const QString &commitMsg);
};

#endif // MAINWIDGET_H
