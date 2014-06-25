#include "osiosmainwindow.h"

#include <QTabWidget>
#include <QLabel>

#include "todolist/todolistwidget.h"
#include "messages/osiosmessagescontactswidget.h"
#include "webbrowser/osioswebbrowserwidget.h"
#include "office/osiosofficenotepadwidget.h"
#include "filebrowser/osiosfilebrowserwidget.h"

//TODOoptional: to make it more mobile/screen-real-estate friendly, the tab->sub-tab structure could instead be [abstracted into] swipe-left-for-back type behavior (or just "back" button for "up/left")
OsiosMainWindow::OsiosMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    //TODOoptimization: queued/deferred gui might be worthwhile just here for this mainwindow-with-tons-of-tab-children

    setUpdatesEnabled(false);
    setWindowTitle("OSiOS"); // osIos, Osios, OSIOS, OsIos

    //TODOreq: "user profile" drop down, or perhaps user profile tabs! either one works, one is just more memory conservative (TODOoptimiation: lazy load all ui/widget stuff and it would help a lot (like, additionally, just don't deserialize all of their session blob until needed) if using user profile tabs). TODOreq: "new user", "delete user" (undoable). It makes sense to me that there's an "all users" tab (everyone on DHT would be swell, but everyone on local machine would be a better default) and then when you click on a user, a new neighbor tab opens and all of that user's session goes in their

    QTabWidget *tabWidget = new QTabWidget();

    int firstTab = tabWidget->addTab(new QWidget(), tr("Actions Timeline")); //TODOreq: horizontal or vertical scroller with custom/memory-efficient "pre-scrolling" that keeps up with the user under most circumstances (find the balance). Also jump POINTS (begin,end,random) or jump AMOUNTS (earlier 5 docs, earlier 5 mins, later too), etc

    //TO DOnereq: a todo list enterer and processor would be great, but i wonder if it should integrate directly into timeline view. actually methinks not, but still worth noting i once thought it
    tabWidget->addTab(new TodoListWidget(), tr("To Do"));

    //Messages
    QTabWidget *osiosMessagesWidget = new QTabWidget(); //had this as a qwidget with a hasA qtabwidget, but i can always convert back to that WHEN I NEED TO (if ever)
    tabWidget->addTab(osiosMessagesWidget, tr("Messages"));

    //Messages sub-tabs
    osiosMessagesWidget->addTab(new OsiosMessagesContactsWidget(), tr("Contacts"));
    QTabWidget *osiosMessagesConversationsWidget = new QTabWidget(); //individual conversations make up the tabs, can convert to qwidget that hasA tabWidget with ease if needed
    osiosMessagesWidget->addTab(osiosMessagesConversationsWidget, tr("Conversations")); //TODOreq: make all tabs have tabwidget as parent. not huge deal i don't think
    osiosMessagesConversationsWidget->addTab(new QLabel(tr("Go to the Contacts tab to begin a conversation")), tr("No Conversations Yet")); //TODOreq: deletes when either [first] message is received or [first] message to be sent is about to start being written ("Send Message" action on contacts widget). reappears if all messages "hidden" (nothing deleteable, so I guess archive is correct wording?)

    tabWidget->addTab(new OsiosWebBrowserWidget(), tr("Web Browser"));
    tabWidget->addTab(new OsiosOfficeNotepadWidget(), tr("Writer"));
    tabWidget->addTab(new OsiosFileBrowserWidget(), tr("File Browser"));
    tabWidget->addTab(new QWidget(), tr("File Viewer"));
    tabWidget->addTab(new QWidget(), tr("File Importer")); //TODOreq: bulk import. TODOreq: File export (perhaps a right click function in both file browser and file viewer)
    //TODOoptional: could put all of these under "file" tabwidget: browser, viewer, importer, exporter. Hell even timeline view kind of belongs beneath it

    //TODOlater: design equals implementation integration, inline editable

    tabWidget->setCurrentIndex(firstTab);
    setCentralWidget(tabWidget);
    setUpdatesEnabled(true);
}
