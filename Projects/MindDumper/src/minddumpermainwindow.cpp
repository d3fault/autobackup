#include "minddumpermainwindow.h"

#include <QApplication>
#include <QFileInfo>
#include <QSettings>
#include <QTabWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStyle>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

#include "minddumpdocument.h"

//TODOreq: asterisk in tab title when unsaved, asterisk in window title when any tab unsaved
MindDumperMainWindow::MindDumperMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_TabTitleAutoNumber(-1)
{
    QStringList argz = qApp->arguments();
    argz.removeFirst(); //app filename
    if(argz.size() != 1)
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("The first argument provided to this application must be the path of your minddump directory. Quitting"));
        doQueuedClose();
        return;
    }
    m_MindDumpDirectoryWithSlashAppended = appendSlashIfNeeded(argz.takeFirst());
    QFileInfo fileInfo(m_MindDumpDirectoryWithSlashAppended);
    if(!fileInfo.isDir())
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Path either does not exist or is not a directory: ") + m_MindDumpDirectoryWithSlashAppended);
        doQueuedClose();
        return;
    }

    QCoreApplication::setOrganizationName("MindDumperOrganization");
    QCoreApplication::setOrganizationDomain("MindDumperDomain");
    QCoreApplication::setApplicationName("MindDumper");
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    //restoreState(settings.value("windowState").toByteArray());

    m_NewDocumentAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), tr("&New"), this);
    //m_NewDocumentAction->setShortcut(QKeySequence::New);
    m_NewDocumentAction->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_N) << QKeySequence(Qt::CTRL + Qt::Key_T));
    connect(m_NewDocumentAction, SIGNAL(triggered()), this, SLOT(newDocumentAction()));

    m_SaveCurrentTabAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Save"), this);
    //m_SaveCurrentTabAction->setShortcut(QKeySequence::Save);
    m_SaveCurrentTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(m_SaveCurrentTabAction, SIGNAL(triggered()), this, SLOT(saveCurrentTabAction()));

    m_SaveAllTabsAction = new QAction(style()->standardIcon(QStyle::SP_DialogYesButton), tr("Save &All"), this);
    //m_SaveAllTabsAction->setShortcut(QKeySequence::SaveAs); //My guess is that save-all is ctrl+shift+s? <-nope
    m_SaveAllTabsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    connect(m_SaveAllTabsAction, SIGNAL(triggered()), this, SLOT(saveAllTabsAction()));

    m_SaveCurrentTabThenCloseCurrentTabAction = new QAction(tr("[Save and] Close Current Tab"), this);
    m_SaveCurrentTabThenCloseCurrentTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(m_SaveCurrentTabThenCloseCurrentTabAction, SIGNAL(triggered()), this, SLOT(saveCurrentTabThenCloseCurrentTabAction()));

    m_SaveAllTabsThenCloseAllTabsAction = new QAction(tr("[Save and] Close All Tabs"), this);
    m_SaveAllTabsThenCloseAllTabsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_W));
    connect(m_SaveAllTabsThenCloseAllTabsAction, SIGNAL(triggered()), this, SLOT(saveAllTabsThenCloseAllTabsAction()));

    m_QuitAction = new QAction(tr("[Save All Tabs and] &Quit"), this);
    m_QuitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(m_QuitAction, SIGNAL(triggered()), this, SLOT(doQueuedClose()));

    m_SaveCurrentTabThenOpenNewDocumentAction = new QAction(tr("Save Current, Open New"), this);
    m_SaveCurrentTabThenOpenNewDocumentAction->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N) << QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(m_SaveCurrentTabThenOpenNewDocumentAction, SIGNAL(triggered()), this, SLOT(saveCurrentTabThenOpenNewDocumentAction()));

    m_NextTabAction = new QAction(style()->standardIcon(QStyle::SP_ArrowForward), tr("Next Tab"), this);
    m_NextTabAction->setShortcut(Qt::CTRL + Qt::Key_Tab);
    connect(m_NextTabAction, SIGNAL(triggered()), this, SLOT(nextTabAction()));

    m_PreviousTabAction = new QAction(style()->standardIcon(QStyle::SP_ArrowBack), tr("Previous Tab"), this);
    m_PreviousTabAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab);
    connect(m_PreviousTabAction, SIGNAL(triggered()), this, SLOT(previousTabAction()));

    //OLD comment:
    //^My use of bool flags is sloppy/ugly. It works, but as I add more actions the complexity grows exponentially and I'm bound to forget to set some bool to false during some corner case and poof there goes my files. I'm considering using actions in scoped pointers, eg m_ActionToPerformAfterSaveAttemptFinishes could point to a "saveNextUnsavedThenContinueOn" action, and when THAT action finishes then I check if an m_ActionToPerformAfterSaveAllTabsActionFinishes isn't null, and if it isn't then I do that action. One such action there would be "quit". But I know deep down I'm reinventing finite state machines. I know Qt has a framework for this! I'm just not used to using them. I remember doing FSM in college when we built a CPU out of NAND gates [on a simulator], but I've not used FSMs since. I want to, and I've read the Qt documentation on them like 20 fucking times, but each time i do I forget wtf is going on and/or forget my original task. So here it is, this comment. Come back to it after you read the FSM doc yet again.
    //^^yep ok after over an hour of reading the FSM doc I'm still amazed by it's power but unable to connect the pieces to be able to utilize it in my code, guh. I'm just gonna use the scoped pointer qaction reinvention, because at least I understand [how to use] it

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_NewDocumentAction);
    fileMenu->addAction(m_SaveCurrentTabThenOpenNewDocumentAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_SaveCurrentTabAction);
    fileMenu->addAction(m_SaveAllTabsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_SaveCurrentTabThenCloseCurrentTabAction);
    fileMenu->addAction(m_SaveAllTabsThenCloseAllTabsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_QuitAction);

    QToolBar *toolBar = addToolBar(tr("File Actions Toolbar"));
    toolBar->addAction(m_NewDocumentAction);
    toolBar->addSeparator();
    toolBar->addAction(m_SaveCurrentTabAction);
    toolBar->addAction(m_SaveAllTabsAction);
    toolBar->addSeparator();
    toolBar->addAction(m_PreviousTabAction);
    toolBar->addAction(m_NextTabAction);

    setCentralWidget(m_TabWidget = new QTabWidget());
    m_TabWidget->setDocumentMode(true);
    connect(m_TabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentTabIndexChanged(int)));
    //this is handled implicitly in handleCurrentTabIndexChanged: newDocumentAction();
    handleCurrentTabIndexChanged(-1); //get things started

    //actually yea we just delay the close itself until saving completes [without error]. qApp->setQuitOnLastWindowClosed(false);
    //no such signal lol, closeEvent instead. connect(this, SIGNAL(closed))
}
void MindDumperMainWindow::closeEvent(QCloseEvent *theCloseEvent)
{
    int numTabs = m_TabWidget->count();
    for(int i = 0; i < numTabs; ++i)
    {
        MindDumpDocument *currentMindDumpDocument = qobject_cast<MindDumpDocument*>(m_TabWidget->widget(i));
        if(!currentMindDumpDocument)
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("Close Event triggered, but we failed to get the widget at TAB WIDGET INDEX (not tab title): ") + QString::number(i) + tr(". This is probably a bug"));
            theCloseEvent->ignore();
            return;
        }
        if(!ensureSavedIfNotEmpty(currentMindDumpDocument))
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("During a Close Event, we failed to ensure the tab is saved if not empty: ") + currentMindDumpDocument->tabTitle());
            theCloseEvent->ignore();
            return;
        }
    }

    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    //settings.setValue("windowState", saveState());

    QMainWindow::closeEvent(theCloseEvent);
}
MindDumpDocument *MindDumperMainWindow::createAndAddMindDumpDocument()
{
    QString tabTitle = getTabTitleAutoNumber();
    MindDumpDocument *newDocument;
    m_TabWidget->addTab(newDocument = new MindDumpDocument(tabTitle, m_MindDumpDirectoryWithSlashAppended, this), tabTitle);
    return newDocument;
}
QString MindDumperMainWindow::getTabTitleAutoNumber()
{
    return QString::number(++m_TabTitleAutoNumber);
}
bool MindDumperMainWindow::ensureSavedIfNotEmpty(MindDumpDocument *mindDumpDocument)
{
    if(mindDumpDocument->isSaved() || mindDumpDocument->isEmpty())
        return true;
    return mindDumpDocument->saveAndFudgeLastModifiedTimestamp();
}
void MindDumperMainWindow::newDocumentAction()
{
    //resetAllPostActionActions();
    MindDumpDocument *newDocument = createAndAddMindDumpDocument();
    m_TabWidget->setCurrentWidget(newDocument);
    newDocument->setFocusOnDocument();
}
void MindDumperMainWindow::saveCurrentTabAction()
{
    MindDumpDocument *currentTab = qobject_cast<MindDumpDocument*>(m_TabWidget->currentWidget());
    if(!currentTab)
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Save Current Tab action triggered, but there is no current tab. This is probably a bug"));
        return;
    }
    if(currentTab->isEmpty())
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Save Current Tab action triggered, but there it is an empty document. This is probably a bug"));
        return;
    }
    if(currentTab->isSaved())
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Save Current Tab action triggered, but there it is already saved. This is probably a bug"));
        return;
    }
    if(!currentTab->saveAndFudgeLastModifiedTimestamp())
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to save current tab"));
        return;
    }
}
void MindDumperMainWindow::saveAllTabsAction()
{
    bool atLeastOneSaved = false;

    int numTabs = m_TabWidget->count();
    for(int i = 0; i < numTabs; ++i)
    {
        MindDumpDocument *currentMindDumpDocument = qobject_cast<MindDumpDocument*>(m_TabWidget->widget(i));
        if(!currentMindDumpDocument)
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("Save All Tabs action triggered, but we failed to get the widget at TAB WIDGET INDEX (not tab title): ") + QString::number(i) + tr(". This is probably a bug"));
            return;
        }
        if(ensureSavedIfNotEmpty(currentMindDumpDocument))
        {
            atLeastOneSaved = true;
        }
        else
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("During a Save All Tabs action, we failed to ensure the tab is saved if not empty: ") + currentMindDumpDocument->tabTitle());
            return;
        }
    }

    if(!atLeastOneSaved)
    {
        QMessageBox::warning(this, tr("Warning!"), tr("Save All Tabs action triggered and each tab visitted without error, but none of the tabs were saved. This is probably a bug"));
        return;
    }

    QMessageBox::information(this, tr("Success!"), tr("All tabs have been saved successfully"));
}
void MindDumperMainWindow::saveCurrentTabThenCloseCurrentTabAction()
{
    int currentIndex = m_TabWidget->currentIndex();
    MindDumpDocument *mindDumpDocument = qobject_cast<MindDumpDocument*>(m_TabWidget->widget(currentIndex));
    if(!mindDumpDocument)
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Save Current Tab Then Close Current Tab action triggered, but there is no current tab. This is probably a bug"));
        return;
    }
    if(!ensureSavedIfNotEmpty(mindDumpDocument))
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("During a Save Current Tab Then Close Current Tab action, we failed to ensure the tab is saved if not empty"));
        return;
    }
    //it's either saved or it's emtpy
    m_TabWidget->removeTab(currentIndex);
    delete mindDumpDocument;
}
void MindDumperMainWindow::saveAllTabsThenCloseAllTabsAction()
{
    int numTabs = m_TabWidget->count();
    for(int i = 0; i < numTabs; ++i)
    {
        MindDumpDocument *currentMindDumpDocument = qobject_cast<MindDumpDocument*>(m_TabWidget->widget(i));
        if(!currentMindDumpDocument)
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("Save All Tabs Then Close All Tabs action triggered, but we failed to get the widget at TAB WIDGET INDEX (not tab title): ") + QString::number(i) + tr(". This is probably a bug"));
            return;
        }
        if(!ensureSavedIfNotEmpty(currentMindDumpDocument))
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("During a Save All Tabs Then Close All Tabs action, we failed to ensure the tab is saved if not empty: ") + currentMindDumpDocument->tabTitle());
            return;
        }
    }

    //this caused infinite loop because handleCurrentTabIndexChanged spawns a new one when count reaches 0 (I could have also made the connection queued, but meh): while(m_TabWidget->count() > 0)
    for(int i = 0; i < numTabs; ++i)
    {
        MindDumpDocument *mindDumpDocument = qobject_cast<MindDumpDocument*>(m_TabWidget->widget(0));
        m_TabWidget->removeTab(0);
        delete mindDumpDocument;
    }
}
void MindDumperMainWindow::saveCurrentTabThenOpenNewDocumentAction()
{
    MindDumpDocument *currentTab = qobject_cast<MindDumpDocument*>(m_TabWidget->currentWidget());
    if(!currentTab)
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Save Current Tab Then Open New Document action triggered, but there is no current tab. This is probably a bug"));
        return;
    }
    if(!ensureSavedIfNotEmpty(currentTab))
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("During a Save Current Tab Then Open New Document action, we failed to ensure saved if not empty: ") + currentTab->tabTitle());
        return;
    }
    newDocumentAction();
}
void MindDumperMainWindow::nextTabAction()
{
    //loop around, but don't set to index already at
    int currentIndex = m_TabWidget->currentIndex();
    int maybeNextIndex = currentIndex + 1;
    if(maybeNextIndex == m_TabWidget->count())
        maybeNextIndex = 0;
    if(currentIndex == maybeNextIndex)
        return;
    m_TabWidget->setCurrentIndex(maybeNextIndex);
}
void MindDumperMainWindow::previousTabAction()
{
    //loop around, but don't set to index already at
    int currentIndex = m_TabWidget->currentIndex();
    int maybePreviousIndex = currentIndex - 1;
    if(maybePreviousIndex < 0)
        maybePreviousIndex = m_TabWidget->count() - 1;
    if(currentIndex == maybePreviousIndex)
        return;
    m_TabWidget->setCurrentIndex(maybePreviousIndex);
}
void MindDumperMainWindow::handleCurrentTabIndexChanged(int newCurrentTabIndex)
{
    if(newCurrentTabIndex == -1)
    {
        newDocumentAction();
        return; //we return because the newDocumentAction will trigger yet another handleCurrentTabIndexChanged
    }

    //TODOreq: enable/disable actions based on saved'ness and emptiness
}
void MindDumperMainWindow::doQueuedClose()
{
    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
}
