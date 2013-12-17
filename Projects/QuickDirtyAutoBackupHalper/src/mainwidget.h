#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QDateTime>
#include <QPlainTextEdit>
#include <QLabel>
#include <QHash>
#include <QList>
#include <QFileDialog>
#include <QPair>
#include <QSettings>

class QuickDirtyAutoBackupHalperBusiness;

//TODOreq: handle the case where i launch with no config file, then add via browse a container that's already mounted. i should just error out i guess? before even allowing them to select a password file... but how do i check that on the gui lol?

#define COMMIT_MESSAGE_DATE_SPECIAL_REPLACE_STRING "%DATETIME%"

class mainWidget : public QWidget
{
    Q_OBJECT
public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();
    void setBusiness(QuickDirtyAutoBackupHalperBusiness *business);
private:
    QHBoxLayout *createNewTcRow(const QString &containerPath = QString(), const QString &passwordPath = QString());
    void rePopulateListFromGuiRowsExcludingEmpty();
    void rePopulateNameFilterListFromGui();
    void addCommaSeparatedValuesToListFromLineEdit(QLineEdit *commaSeparatedValuesLineEdit, QStringList *listToPutNonEmptyValuesInto);
    QuickDirtyAutoBackupHalperBusiness *m_Business;
    inline void connectToBusiness();

    //gui
    inline void setupGui();
    inline void enableDisableMountDismountButtonsAndRowLineEditChangeability(bool mounted);
    QVBoxLayout *m_Layout;
    QCheckBox *m_ReadOnlyCheckBox; //TODOoptional: contingent on if read only is checked, another checkbox called 'mount only first' [container]. because when mounting as read only i will be using it for browsing and yea i only need one. the dismount needs to take this into account as well so it doesn't try to dismount non-mounted containers (not that it would matter, truecrypt handles that fine with an error 'not mounted')
    QLineEdit *m_FilenameIgnoreLineEdit;
    QLineEdit *m_FilenamesEndsWithIgnoreLineEdit;
    QLineEdit *m_DirnameIgnoreLineEdit;
    QLineEdit *m_DirnameEndsWithIgnoreLineEdit;
    QPushButton *m_PushToGitIgnoreButton;
    QLineEdit *m_WorkingDirectoryLineEdit;
    QLineEdit *m_MountPointSubDirToBareGitRepoLineEdit;
    QLineEdit *m_DirStructureFileNameLineEdit;
    QWidget *m_RowsPlaceholderWidget;
    QVBoxLayout *m_RowsLayout;
    QPushButton *m_AddTcRowButton;
    QPushButton *m_PersistTcRowsButton;
    QPushButton *m_MountTcRowsButton;
    QPushButton *m_DismountTcRowsButton;
    QPushButton *m_ClearCommitMessageButton;
    QLineEdit *m_CommitMessageLineEdit;
    QCheckBox *m_PushEvenWhenNothingToCommitCheckbox;
    QCheckBox *m_CommitIfNeededBeforeShuttingDownCheckbox;
    QPushButton *m_CommitAfterMountingIfNeededButton;
    QPushButton *m_ShutdownAfterDismountingIfNeededButton;
    QPlainTextEdit *m_Debug;

    QList<QPair<QLineEdit*, QLineEdit*> > m_TcRowsList;
    QHash<QPushButton*, QLineEdit*> m_BrowseButtonLineEditAssociations;

    QStringList *m_FilenameIgnoreList;
    QStringList *m_FilenameEndsWithIgnoreList;
    QStringList *m_DirnameIgnoreList;
    QStringList *m_DirnameEndsWithIgnoreList;

    QString m_DateTimeSpecialString;
    QString m_CancelShutdownText;
    QString m_ShutdownText;
    bool m_BrokenOverride;
    bool m_ShuttingDown;

    QList<QPair<QString,QString> > *m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit; //i wanted to create/populate this on the stack inside handleMountButtonClicked... but then it would go out of scope and be a null pointer. I don't like passing non-pointers (except strings)
signals:
    void pushToGitIgnoreRequested(const QString&workingDir, QStringList *filenamesIgnoreList, QStringList *filenamesEndsWithIgnoreList, QStringList *dirnamesIgnoreList, QStringList *dirnamesEndsWithIgnoreList);
    void persistRequested(QList<QPair<QString,QString> > *deviceAndPasswordPaths);
    void mountRequested(QList<QPair<QString,QString> > *deviceAndPasswordPaths, bool mountAsReadOnly);
    //there is no dismountRequested, we are connected directly to the button's clicked() signal
    void commitRequested(const QString &commitMsg, QList<QPair<QString,QString> > *deviceAndPasswordPaths, const QString &workingDirString, const QString &subDirOnMountPointToBareGitRepo, const QString &dirStructureFileNameString, QStringList *filenameIgnoreList, QStringList *filenameEndsWithIgnoreList, QStringList *dirnameIgnoreList, QStringList *dirnameEndsWithIgnoreList, bool pushEvenWhenNothingToCommit);
    void shutdownRequested();
    void cancelShutdownRequested();
private slots:
    void handleD(const QString &msg);
    void handleBrokenStateDetected();
    void handleReadOnlyCheckToggled(bool checked);
    void handlePushToGitIgnoreButtonClicked();
    void handleSettingsReadAndDevicesProbed(bool allMountedTrueOrAllDismountedFalse, bool theyAreMountedAsReadOnlyIfTheyAreMountedAtAll, QList<QPair<QString,QString> > *deviceAndPasswordPathsFromSettingsRegardlessOfMountStatus);
    void addTcRowToGui(const QString &containerPath = QString(), const QString &passwordPath = QString());
    void handlePersistRowsButtonClicked();
    void handleMountButtonClicked();
    void handleMounted();
    void handleDismounted();
    void handleCommitted();
    void clearCommitMessageAndFocusTheLineEdit();
    void handleCommitButtonClicked();
    void handleShutdownButtonClicked();
    void handleBrowseButtonClickedAndFigureOutWhichLineEditToUpdate();
};

#endif // MAINWIDGET_H
