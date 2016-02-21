#include "minddumpermainwindow.h"

#include <QApplication>
#include <QFileInfo>
#include <QTabWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QAction>
#include <QStyle>
#include <QMenuBar>
#include <QMenu>

#include "minddumpdocument.h"

MindDumperMainWindow::MindDumperMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_TabTitleAutoNumber(-1)
    , m_QuitInProgress(false)
    , m_SavingAllTabsActionInProgress(false)
    , m_TabIndexCurrentlyAttemptingToSave(-1)
{
    QStringList argz = qApp->arguments();
    argz.removeFirst(); //app filename
    if(argz.size() != 1)
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("The first argument provided to this application should be the path of your minddump directory. Quitting"));
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    m_MindDumpDirectoryWithSlashAppended = appendSlashIfNeeded(argz.takeFirst());
    QFileInfo fileInfo(m_MindDumpDirectoryWithSlashAppended);
    if(!fileInfo.isDir())
    {
        QMessageBox::critical(this, tr("Critical Error!"), tr("Path either does not exist or is not a directory: ") + m_MindDumpDirectoryWithSlashAppended);
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }

    m_SaveCurrentTabAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Save"), this);
    connect(m_SaveCurrentTabAction, SIGNAL(triggered()), this, SLOT(saveCurrentTabAction()));
    m_SaveAllTabsAction = new QAction(style()->standardIcon(QStyle::SP_DialogYesButton), tr("Save &All"), this);
    connect(m_SaveAllTabsAction, SIGNAL(triggered()), this, SLOT(saveAllTabsAction()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_SaveCurrentTabAction);
    fileMenu->addAction(m_SaveAllTabsAction);

    //QToolBar *toolBar = tool

    setCentralWidget(m_TabWidget = new QTabWidget());
    MindDumpDocument *firstDocument = createAndAddMindDumpDocument();
    firstDocument->setFocusOnDocument();

    //actually yea we just delay the close itself until saving completes [without error]. qApp->setQuitOnLastWindowClosed(false);
    //no such signal lol, closeEvent instead. connect(this, SIGNAL(closed))
}
void MindDumperMainWindow::closeEvent(QCloseEvent *theCloseEvent)
{
    //TODOreq: only don't ignore the close event if all tabs are already saved
    m_QuitInProgress = true;
    m_SavingAllTabsActionInProgress = true;
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedTabIndex(-1);
    if(m_TabIndexCurrentlyAttemptingToSave > -1)
    {
        theCloseEvent->ignore(); //TODOreq: close later
        saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(m_TabIndexCurrentlyAttemptingToSave);
    }
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
int MindDumperMainWindow::getNextUnsavedTabIndex(int fromIndexExcluding)
{
    int tabCount = m_TabWidget->count();
    for(int i = fromIndexExcluding+1; i < tabCount; ++i)
    {
        MindDumpDocument *mindDumpDocument = static_cast<MindDumpDocument*>(m_TabWidget->widget(i));
        if(!mindDumpDocument->isSaved())
            return i;
    }
    return -1;
}
void MindDumperMainWindow::saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(int tabIndexToSave)
{
    MindDumpDocument *mindDumpDocument = static_cast<MindDumpDocument*>(m_TabWidget->widget(tabIndexToSave));
    connect(mindDumpDocument, SIGNAL(savedAndFudgedLastModifiedTimestamp(bool)), this, SLOT(handleMindDumpDocumentSaveAttemptFinished(bool))); //TODOmb: disconnect afterwards? probably won't matter but might if save fails and we re-attempt it :-/
    mindDumpDocument->saveAndFudgeLastModifiedTimestamp();
}
void MindDumperMainWindow::saveCurrentTabAction()
{
    m_SavingAllTabsActionInProgress = false;
    m_TabIndexCurrentlyAttemptingToSave = m_TabWidget->currentIndex();
    if(!static_cast<MindDumpDocument*>(m_TabWidget->widget(m_TabIndexCurrentlyAttemptingToSave))->isSaved())
        saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(m_TabIndexCurrentlyAttemptingToSave);
    else
        qDebug("Save Current Tab Action triggered, but current tab already saved");
}
void MindDumperMainWindow::saveAllTabsAction()
{
    m_SavingAllTabsActionInProgress = true;
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedTabIndex(-1);
    if(m_TabIndexCurrentlyAttemptingToSave > -1)
        saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(m_TabIndexCurrentlyAttemptingToSave);
    else
        qDebug("Save All Tabs Action triggered, but all tabs already saved");
}
void MindDumperMainWindow::handleMindDumpDocumentSaveAttemptFinished(bool success)
{
    MindDumpDocument *mindDumpDocument = qobject_cast<MindDumpDocument*>(sender());
    disconnect(mindDumpDocument, SIGNAL(savedAndFudgedLastModifiedTimestamp(bool)));
    if(!success)
    {
        m_SavingAllTabsActionInProgress = false; //whether or not there was one occuring, we set it to false from here on out. TODOreq: a "quitting after saving all tabs" variable should similarly be set to false here
        m_QuitInProgress = false;
        QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to save tab: ") + mindDumpDocument->tabTitle());
        return;
    }
    mindDumpDocument->setDisabled(true);

    if(!m_SavingAllTabsActionInProgress)
        return;
    m_TabIndexCurrentlyAttemptingToSave = getNextUnsavedTabIndex(m_TabIndexCurrentlyAttemptingToSave);
    if(m_TabIndexCurrentlyAttemptingToSave == -1)
    {
        if(m_QuitInProgress)
        {
            close(); //TODOmb: a progress dialog when saving in bulk (either in app or when quitting)
            return;
        }
        QMessageBox::information(this, tr("Success!"), tr("All tabs have been saved successfully"));
        return;
    }
    saveTabAtIndexThenMoveOntoNextUnsavedTabIfNoErrors(m_TabIndexCurrentlyAttemptingToSave);
}
