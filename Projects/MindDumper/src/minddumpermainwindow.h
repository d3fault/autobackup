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
    ~MindDumperMainWindow();
protected:
    bool handleSignal(int sig);
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
    QAction *m_NextTabAction;
    QAction *m_PreviousTabAction;

    bool ensureSavedIfNotEmpty(MindDumpDocument *mindDumpDocument);

    static inline QString appendSlashIfNeeded(const QString &inputString) { if(inputString.endsWith("/")) return inputString; return inputString + "/"; }
private slots:
    void newDocumentAction();
    void saveCurrentTabAction();
    void saveAllTabsAction();
    void saveCurrentTabThenCloseCurrentTabAction();
    void saveAllTabsThenCloseAllTabsAction();
    void saveCurrentTabThenOpenNewDocumentAction();
    void nextTabAction();
    void previousTabAction();

    void handleCurrentTabIndexChanged(int newCurrentTabIndex);

    void handleSystemSignalInterruptOrTerminateReceived();
    void doQueuedClose();
};

#endif // MINDDUMPERMAINWINDOW_H
