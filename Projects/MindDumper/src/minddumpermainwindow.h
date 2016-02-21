#ifndef MINDDUMPERMAINWINDOW_H
#define MINDDUMPERMAINWINDOW_H

#include <QMainWindow>

class QTabWidget;
class QAction;

class MindDumpDocument;

class MindDumperMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MindDumperMainWindow(QWidget *parent = 0);
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

    bool m_QuitInProgress;
    bool m_SavingAllTabsActionInProgress;
    int m_TabIndexCurrentlyAttemptingToSave;
    int getNextUnsavedTabIndex(int fromIndexExcluding);

    void saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(int tabIndexToSave);

    static inline QString appendSlashIfNeeded(const QString &inputString) { if(inputString.endsWith("/")) return inputString; return inputString + "/"; }
private slots:
    void newDocumentAction();
    void saveCurrentTabAction();
    void saveAllTabsAction();
    void handleMindDumpDocumentSaveAttemptFinished(bool success);
};

#endif // MINDDUMPERMAINWINDOW_H
