#include "osiosfilebrowserwidget.h"

#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QTreeView>

//TODOreq? Even though this APPEARS to be your standard file browser, the files are actually saved using a custom format that allows file moves/deletions/etc to be tracked in timeline view as mere mutations. TODOoptional: fuse/windows-driver integration
//TODOoptional: relates to above req, perhaps just to start we can show a simple list of the notepad docs (TODOreq: in any case, double clicking on it brings you to the 'latest'/replayed-fully version of the file in notepad, right clicking allows you to select timeline view of it)
OsiosFileBrowserWidget::OsiosFileBrowserWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    //TODOreq: tons of tools on a row above. SENDING [to contact], sorting, cut, copy/paste, blah blah blah

    //COMMENTED OUT fileSystemModel usage BECAUSE OF "TODOreq?" at top of file. Will probably need custom model
    //QFileSystemModel *fileSystemModel = new QFileSystemModel(this);
    //fileSystemModel->setRootPath(""); //TODOreq: root of 'files' directory on first launch, save/restore last view on exit/relaunch
    QTreeView *treeView = new QTreeView();
    //treeView->setModel(fileSystemModel);

    treeView->setAnimated(false);
    treeView->setIndentation(5);
    treeView->setSortingEnabled(true);

    myLayout->addWidget(treeView, 1);

    setLayout(myLayout);
}
