#ifndef MINDDUMPERMAINWINDOW_H
#define MINDDUMPERMAINWINDOW_H

#include <QMainWindow>

#include <QHash>
#include <QScopedPointer>
#include <QAction>

class QTabWidget;

class MindDumpDocument;

struct MindDumperPostActionActions
{
    enum MindDumperPostActionActionsEnum
    {
          PostSaveSingleTabAction = 0
        , PostSaveAllTabsAction = 1
    };
};
class MindDumperMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MindDumperMainWindow(QWidget *parent = 0);
    ~MindDumperMainWindow();
protected:
    virtual void closeEvent(QCloseEvent *theCloseEvent);
private:
    QString m_MindDumpDirectoryWithSlashAppended;
    QTabWidget *m_TabWidget;

    MindDumpDocument *createAndAddMindDumpDocument();

    int m_TabTitleAutoNumber;
    QString getTabTitleAutoNumber();

    QAction *m_NewDocumentAction;
    QAction *m_SaveCurrentTabAction;
    QAction *m_SaveAllTabsAction;
    QAction *m_SaveCurrentTabThenCloseCurrentTabAction;
    QAction *m_SaveAllTabsThenCloseAllTabsAction;
    QAction *m_QuitAction;
    QAction *m_SaveCurrentTabThenOpenNewDocumentAction;

    //bool m_QuitInProgress;
    //bool m_SavingAllTabsActionInProgress;
    int m_TabIndexCurrentlyAttemptingToSave;
    int getNextUnsavedAndNonEmptyTabIndex(int fromIndexExcluding);

    QHash<MindDumperPostActionActions::MindDumperPostActionActionsEnum, QScopedPointer<QAction>*> m_PostActionActions;
    void doActionIfAny(MindDumperPostActionActions::MindDumperPostActionActionsEnum actionToDoIfSet);
    void resetAllPostActionActions();

    void saveSingleTabAtIndex(int tabIndexToSave);
    bool closeTabAtIndex(int tabIndexToClose);

    static inline QString appendSlashIfNeeded(const QString &inputString) { if(inputString.endsWith("/")) return inputString; return inputString + "/"; }
private slots:
    void newDocumentAction();
    void saveCurrentTabAction();
    void saveAllTabsAction();
    void saveCurrentTabThenCloseCurrentTabAction();
    void saveAllTabsThenCloseAllTabsAction();
    void saveCurrentTabThenOpenNewDocumentAction();

    void handleMindDumpDocumentSaveAttemptFinished(bool success);

    void saveFirstUnsavedTabThenMoveOntoNextUnsavedTabs_OrDoPostSaveAllTabsAction();

    void closeCurrentTab();
    void closeAllSavedTabs();
    void showAllTabsSavedMessageBox();
    void doQueuedClose();
};

#endif // MINDDUMPERMAINWINDOW_H
