#include "minddumpermainwindow.h"
#include <QApplication>

//Even though it's bad practice, I'm going to have the gui and backend be one. So my "MindDump Document" is going to _BE_ a QPlainTextEdit, etc
//TO DOnereq: maybe Ctrl+S saves the doc and sets it to read-only (grayed out) mode, and Ctrl+Shift+S does that for all tabs (with  confirmation message box at end? because seeing them all grayed out as confirmation would be too much work). Ctrl+W closes current tab, auto-saves ofc. Ctrl+Shift+W cloes all tabs, auto-saves ofc. Only via File->Discard can we close current tab without saving, and that should have a "Are you sure?" dialog TODOreq --- I don't want to allow ANY editting in this app (yet? it just gets complicated with timestamps etc). If you want to edit, you open it up mousepad or whatever. But you 'know' when you do this that you'll be changing the last modified timestamp [again], so you 'know' to commit first if you want the orig one
//TODOfuture: an 'autosave~' functionality, because I could see myself going with lots of tabs open all unsaved and then poof comp crashes and I ragequit life
//TODOreq: in the business logic there's a lot of copy/paste code. that by itself is dangerous, but simultaneously I'm also kinda scared to modify any of this app "if it ain't broke don't fix it"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#error Windows not supported until a suitable `touch` replacement found
    return 1;
#else
    MindDumperMainWindow w;
    w.show();
#endif

    return a.exec();
}
