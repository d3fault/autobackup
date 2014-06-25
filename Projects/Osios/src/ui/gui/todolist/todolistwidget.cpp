#include "todolistwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QListView>

//TODOreq: entering item into todo list is item in timeline view, re-ordering item is too, processing is as well
//TODOreq: each item can have infinite sub items recursively, and an item is only done when all it's children items are done (perhaps this is a light requirement and children can be... forked to other items altogether)
//^^^probably need to not keep the entire qlistview in memory, need one of those custom preloader thingos. for now KISS and keep it all in memory
//TODOoptional: a todo list item can become in effect the basis of a design and code, and should thus be linked as such. a notepad document could as well (and perhaps any file type can. the file is "inspiration" or something along those lines). todo vs notepad is basically just length of what will be written, but todos are obviously 'processed' etc too
//TODOreq: "SEND" i guess also makes sense, though i've never seen it heh
//TODOreq: maybe a calendar mode/tab, BUT maybe not and just have them be recurring todo list items (implies a notification is sent somewhere in GUI (regardless of tab) when it is re-approaching). Perhaps calendar tab on main window IN ADDITION TO said todo list integration, but idk I don't want to clutter the main window
TodoListWidget::TodoListWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *newTodoItemRow = new QHBoxLayout();
    QLineEdit *newTodoItemLineEdit = new QLineEdit();
    //TODOreq:
    //QRadioButton *asapRadio = new QRadioButton(tr("ASAP")); //First/Head
    //QRadioButton *laterButSoonerThanDeferred = new QRadioButton(tr("A little later")); //After any that were a) inserted using "first", b) "re-ordered" up to be within any that were inserted using "first", OR c) others that were also inserted using "A little later"
    //QRadioButton *deferredUntilBored = new QRadioButton(tr("Defer until bored")); //Last/Tail
    //TODOreq: the last chosen of the above three radios is remembered in session
    //TODOoptional: a "random point in time" / apathetic/fun/whatever radio option
    //TODOreq: "Do by [fill in a qdatetime widget]", with "N" warnings each at it's own N.x time before that chosen qdatetime (JOB MODE)
    QPushButton *newTodoItemButton = new QPushButton(tr("Add Todo"));

    newTodoItemRow->addWidget(newTodoItemLineEdit, 1);
    //TODOreq: radios above
    newTodoItemRow->addWidget(newTodoItemButton);

    myLayout->addLayout(newTodoItemRow);
    myLayout->addWidget(new QListView(), 1);

    setLayout(myLayout);
}
