#include "minddumpermainwindow.h"

#include <QApplication>
#include <QFileInfo>
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
    //, m_QuitInProgress(false)
    //, m_SavingAllTabsActionInProgress(false)
    , m_TabIndexCurrentlyAttemptingToSave(-1)
{
    QStringList argz = qApp->arguments();
    argz.removeFirst(); //app filename
    if(argz.size() != 1)
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("The first argument provided to this application should be the path of your minddump directory. Quitting"));
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

    m_NewDocumentAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), tr("&New"), this);
    //m_NewDocumentAction->setShortcut(QKeySequence::New);
    m_NewDocumentAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
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
    m_SaveCurrentTabThenOpenNewDocumentAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    connect(m_SaveCurrentTabThenOpenNewDocumentAction, SIGNAL(triggered()), this, SLOT(saveCurrentTabThenOpenNewDocumentAction()));

    //TODOreq: CTRL+TAB, CTRL+SHIFT+TAB

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

    m_PostActionActions .insert(MindDumperPostActionActions::PostSaveSingleTabAction, new QScopedPointer<QAction>());
    m_PostActionActions .insert(MindDumperPostActionActions::PostSaveAllTabsAction, new QScopedPointer<QAction>());

    setCentralWidget(m_TabWidget = new QTabWidget());
    newDocumentAction();

    //actually yea we just delay the close itself until saving completes [without error]. qApp->setQuitOnLastWindowClosed(false);
    //no such signal lol, closeEvent instead. connect(this, SIGNAL(closed))
}
MindDumperMainWindow::~MindDumperMainWindow()
{
    qDeleteAll(m_PostActionActions);
}
void MindDumperMainWindow::closeEvent(QCloseEvent *theCloseEvent)
{
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedAndNonEmptyTabIndex(-1);
    if(m_TabIndexCurrentlyAttemptingToSave > -1)
    {
        //m_QuitInProgress = true;
        //m_SavingAllTabsActionInProgress = true;
        resetAllPostActionActions();
        QAction *postSaveAllTabsAction;
        QScopedPointer<QAction> *postSaveAllTabsActionScopedPointer = m_PostActionActions.value(MindDumperPostActionActions::PostSaveAllTabsAction);
        postSaveAllTabsActionScopedPointer->reset(postSaveAllTabsAction = new QAction(this));
        connect(postSaveAllTabsAction, SIGNAL(triggered()), this, SLOT(doQueuedClose()));

        theCloseEvent->ignore(); //we async save all, then close (again) later
        //saveSingleTabAtIndex(m_TabIndexCurrentlyAttemptingToSave);
        saveFirstUnsavedTabThenMoveOntoNextUnsavedTabs_OrDoPostSaveAllTabsAction();
        return;
    }

    //since the event was not ignored, the window closes and the application quits
}
MindDumpDocument *MindDumperMainWindow::createAndAddMindDumpDocument()
{
    QString tabTitle = getTabTitleAutoNumber();
    MindDumpDocument *newDocument;
    m_TabWidget->addTab(newDocument = new MindDumpDocument(tabTitle, m_MindDumpDirectoryWithSlashAppended), tabTitle);
    return newDocument;
}
QString MindDumperMainWindow::getTabTitleAutoNumber()
{
    return QString::number(++m_TabTitleAutoNumber);
}
int MindDumperMainWindow::getNextUnsavedAndNonEmptyTabIndex(int fromIndexExcluding)
{
    int tabCount = m_TabWidget->count();
    for(int i = fromIndexExcluding+1; i < tabCount; ++i)
    {
        MindDumpDocument *mindDumpDocument = static_cast<MindDumpDocument*>(m_TabWidget->widget(i));
        if((!mindDumpDocument->isSaved()) && (!mindDumpDocument->isEmpty()))
            return i;
    }
    return -1;
}
void MindDumperMainWindow::doActionIfAny(MindDumperPostActionActions::MindDumperPostActionActionsEnum actionToDoIfSet)
{
    QScopedPointer<QAction> *actionToMaybeDo = m_PostActionActions.value(actionToDoIfSet);
    if(actionToMaybeDo->isNull())
        return;
    //actionToMaybeDo->data()->trigger();
    //TO DOnereq: do I want to reset it now that it's been triggered? my head hurts. maybe I want to take() it before trigger()'ing it, because trigger()'ing it might reset it (OR NOT) but we do want to reset it, but without overwriting a new post action action that might have been [re]set after the trigger that this post action action triggered? yea eat my dust
    QScopedPointer<QAction> newOwner(actionToMaybeDo->take()); //never let a heap allocated type go without an owner for even a single statement!
    newOwner->trigger();
    //reset called implicitly in destructor of newOwner
}
void MindDumperMainWindow::resetAllPostActionActions()
{
    Q_FOREACH(QScopedPointer<QAction> *currentPostActionAction, m_PostActionActions)
    {
        currentPostActionAction->reset();
    }
}
void MindDumperMainWindow::saveSingleTabAtIndex(int tabIndexToSave)
{
    MindDumpDocument *mindDumpDocument = static_cast<MindDumpDocument*>(m_TabWidget->widget(tabIndexToSave));
    //TO DOnereq: maybe check if it's already saved? Ideally the actions would be untriggerable and we'd never get here. If it is already saved, do we then do our post single save action?
    if(!mindDumpDocument->isSaved())
    {
        connect(mindDumpDocument, SIGNAL(savedAndFudgedLastModifiedTimestamp(bool)), this, SLOT(handleMindDumpDocumentSaveAttemptFinished(bool)));
        mindDumpDocument->saveAndFudgeLastModifiedTimestamp();
    }
    else
    {
        QMessageBox::warning(this, tr("Warning!"), tr("Save tab called for tab") + mindDumpDocument->tabTitle() + tr(", but it was already saved"));
        doActionIfAny(MindDumperPostActionActions::PostSaveSingleTabAction);
    }
}
bool MindDumperMainWindow::closeTabAtIndex(int tabIndexToClose)
{
    QScopedPointer<MindDumpDocument> minddumpDocument(static_cast<MindDumpDocument*>(m_TabWidget->widget(tabIndexToClose)));
    if((!minddumpDocument->isSaved()) && (!minddumpDocument->isEmpty()))
    {
        resetAllPostActionActions();
        QMessageBox::critical(this, tr("Critical Error!"), tr("Tab ") + minddumpDocument->tabTitle() + tr(" was told to close, but hasn't been saved yet. This is probably a bug. We're leaving the tab open"));
        minddumpDocument.take();
        return false;
    }
    m_TabWidget->removeTab(tabIndexToClose);

    //TO DOnereq: if this was the last tab open, open a new/blank empty one just like at the start of the app
    if(m_TabWidget->count() == 0)
        QMetaObject::invokeMethod(this, "newDocumentAction", Qt::QueuedConnection); //queued because the "close all tabs" loop becomes an infinite loop if it's not queued

    //MindDumpDocument itself gets deleted in scoped pointer destructor
    return true;
}
void MindDumperMainWindow::newDocumentAction()
{
    resetAllPostActionActions();
    MindDumpDocument *newDocument = createAndAddMindDumpDocument();
    m_TabWidget->setCurrentWidget(newDocument);
    newDocument->setFocusOnDocument();
}
void MindDumperMainWindow::saveCurrentTabAction()
{
    resetAllPostActionActions();
    //m_SavingAllTabsActionInProgress = false;
    //m_QuitInProgress = false;
    m_TabIndexCurrentlyAttemptingToSave = m_TabWidget->currentIndex();
    saveSingleTabAtIndex(m_TabIndexCurrentlyAttemptingToSave);
}
void MindDumperMainWindow::saveAllTabsAction()
{
    resetAllPostActionActions();
    QAction *postSaveAllTabsAction;
    QScopedPointer<QAction> *postSaveAllTabsActionScopedPointer = m_PostActionActions.value(MindDumperPostActionActions::PostSaveAllTabsAction);
    postSaveAllTabsActionScopedPointer->reset(postSaveAllTabsAction = new QAction(this));
    connect(postSaveAllTabsAction, SIGNAL(triggered()), this, SLOT(showAllTabsSavedMessageBox()));

    Q_ASSERT(getNextUnsavedAndNonEmptyTabIndex(-1) != -1); //the action should bhave been disabled
    saveFirstUnsavedTabThenMoveOntoNextUnsavedTabs_OrDoPostSaveAllTabsAction();

#if 0
    //m_SavingAllTabsActionInProgress = true;
    //m_QuitInProgress = false;
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedTabIndex(-1);
    if(m_TabIndexCurrentlyAttemptingToSave > -1)
        saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(m_TabIndexCurrentlyAttemptingToSave);
    else
        qDebug("Save All Tabs Action triggered, but all tabs already saved");
#endif
}
void MindDumperMainWindow::saveCurrentTabThenCloseCurrentTabAction()
{
    resetAllPostActionActions();

    m_TabIndexCurrentlyAttemptingToSave = m_TabWidget->currentIndex();
    MindDumpDocument *mindDumpDocument = static_cast<MindDumpDocument*>(m_TabWidget->widget(m_TabIndexCurrentlyAttemptingToSave));
    if(mindDumpDocument->isSaved() || mindDumpDocument->isEmpty())
    {
        closeCurrentTab();
        return;
    }

    QAction *postSaveSingleTabAction;
    QScopedPointer<QAction> *postSaveSingleTabActionScopedPointer = m_PostActionActions.value(MindDumperPostActionActions::PostSaveSingleTabAction);
    postSaveSingleTabActionScopedPointer->reset(postSaveSingleTabAction = new QAction(this));
    connect(postSaveSingleTabAction, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    saveSingleTabAtIndex(m_TabIndexCurrentlyAttemptingToSave);
}
void MindDumperMainWindow::saveAllTabsThenCloseAllTabsAction()
{
    //TO DOnereq: two empty tabs, ctrl+shift+w = infinite loop
    resetAllPostActionActions();
    QAction *postSaveAllTabsAction;
    m_PostActionActions.value(MindDumperPostActionActions::PostSaveAllTabsAction)->reset(postSaveAllTabsAction = new QAction(this));
    connect(postSaveAllTabsAction, SIGNAL(triggered()), this, SLOT(closeAllSavedTabs()));

    //TO DOnereq: after all tabs have closed, open a new/blank empty one just like at the start of the app..... perhaps using a new PostActionAction "PostAllTabsClosedAction"
    saveFirstUnsavedTabThenMoveOntoNextUnsavedTabs_OrDoPostSaveAllTabsAction();
}
void MindDumperMainWindow::saveCurrentTabThenOpenNewDocumentAction()
{
    int currentIndex = m_TabWidget->currentIndex();

    MindDumpDocument *currentTab = static_cast<MindDumpDocument*>(m_TabWidget->widget(currentIndex));
    if(currentTab->isSaved() || currentTab->isEmpty())
    {
        //TO DOnereq: should behave just like "new tab" if current tab is already saved. Atm this will cause a "double save", which is considered a "save error"
        newDocumentAction();
        return;
    }

    resetAllPostActionActions();
    QAction *postSaveSingleTabAction;
    m_PostActionActions.value(MindDumperPostActionActions::PostSaveSingleTabAction)->reset(postSaveSingleTabAction = new QAction(this));
    connect(postSaveSingleTabAction, SIGNAL(triggered()), this, SLOT(newDocumentAction()));

    saveSingleTabAtIndex(currentIndex);
}
void MindDumperMainWindow::handleMindDumpDocumentSaveAttemptFinished(bool success)
{
    MindDumpDocument *mindDumpDocument = qobject_cast<MindDumpDocument*>(sender());
    disconnect(mindDumpDocument, SIGNAL(savedAndFudgedLastModifiedTimestamp(bool)));
    if(!success)
    {
        //m_SavingAllTabsActionInProgress = false; //whether or not there was one occuring, we set it to false from here on out
        //m_QuitInProgress = false;
        resetAllPostActionActions();
        QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to save tab: ") + mindDumpDocument->tabTitle());
        return;
    }
    mindDumpDocument->setDisabled(true);

    doActionIfAny(MindDumperPostActionActions::PostSaveSingleTabAction);

 #if 0 //TODOreq: merge
    if(!m_SavingAllTabsActionInProgress)
        return;
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedTabIndex(m_TabIndexCurrentlyAttemptingToSave);
    if(m_TabIndexCurrentlyAttemptingToSave == -1)
    {
        if(m_QuitInProgress)
        {
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection); //Queued because a closeEvent might have gotten us here, and I think there's some internal qt logic to eat subsequent close() calls or something because calling close() directly didn't work. TODOmb: a progress dialog when saving in bulk (either in app or when quitting)
            return;
        }
        QMessageBox::information(this, tr("Success!"), tr("All tabs have been saved successfully"));
        return;
    }
    saveSingleTabAtIndex(m_TabIndexCurrentlyAttemptingToSave);
#endif
}
void MindDumperMainWindow::saveFirstUnsavedTabThenMoveOntoNextUnsavedTabs_OrDoPostSaveAllTabsAction()
{
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedAndNonEmptyTabIndex(-1);
    if(m_TabIndexCurrentlyAttemptingToSave > -1)
    {
        QAction *postSaveSingleTabAction;
        QScopedPointer<QAction> *postSaveSingleTabActionScopedPointer = m_PostActionActions.value(MindDumperPostActionActions::PostSaveSingleTabAction);
        postSaveSingleTabActionScopedPointer->reset(postSaveSingleTabAction = new QAction(this));
        connect(postSaveSingleTabAction, SIGNAL(triggered()), this, SLOT(saveFirstUnsavedTabThenMoveOntoNextUnsavedTabs_OrDoPostSaveAllTabsAction()));

        saveSingleTabAtIndex(m_TabIndexCurrentlyAttemptingToSave);
    }
    else
        doActionIfAny(MindDumperPostActionActions::PostSaveAllTabsAction);
}
void MindDumperMainWindow::closeCurrentTab()
{
    closeTabAtIndex(m_TabWidget->currentIndex());
}
void MindDumperMainWindow::closeAllSavedTabs()
{
    while(m_TabWidget->count() > 0)
    {
        if(!closeTabAtIndex(0))
            return;
    }

    //hmmm... could have a "post all tabs closed" action, but.....
    //nvm we do it in closeTabAtIndex: newDocumentAction(); //.....atm there's only one thing we ever do after all tabs get closed
}
void MindDumperMainWindow::showAllTabsSavedMessageBox()
{
    QMessageBox::information(this, tr("Success!"), tr("All tabs have been saved successfully"));
}
void MindDumperMainWindow::doQueuedClose()
{
    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
}
