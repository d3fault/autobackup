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
    m_NewTodoItemLineEdit = new QLineEdit();
    //TODOreq:
    //QRadioButton *asapRadio = new QRadioButton(tr("ASAP")); //First/Head
    //QRadioButton *laterButSoonerThanDeferred = new QRadioButton(tr("A little later")); //After any that were a) inserted using "first", b) "re-ordered" up to be within any that were inserted using "first", OR c) others that were also inserted using "A little later"
    //QRadioButton *deferredUntilBored = new QRadioButton(tr("Defer until bored")); //Last/Tail
    //TODOreq: the last chosen of the above three radios is remembered in session
    //TODOoptional: a "random point in time" / apathetic/fun/whatever radio option
    //TODOreq: "Do by [fill in a qdatetime widget]", with "N" warnings each at it's own N.x time before that chosen qdatetime (JOB MODE)
    QPushButton *newTodoItemButton = new QPushButton(tr("Add Todo"));

    newTodoItemRow->addWidget(m_NewTodoItemLineEdit, 1);
    //TODOreq: radios above
    newTodoItemRow->addWidget(newTodoItemButton);

    myLayout->addLayout(newTodoItemRow);
    myLayout->addWidget(new QListView(), 1);

    setLayout(myLayout);

    connect(m_NewTodoItemLineEdit, SIGNAL(returnPressed()), this, SLOT(addNewTodoItem()));
    connect(newTodoItemButton, SIGNAL(clicked()), this, SLOT(addNewTodoItem()));
}
void TodoListWidget::addNewTodoItem()
{
    //see this is what i mean by separating gui/core. i really should turn the new todo item into a todo item object and send it to my backend, my backend should then decide, based on my previously reported "view" (scrollbar positions in this example), whether or not to send it back for presenting. it would be trivial to add it to the qlistview right here/now though (wouldn't be persisted so meh)

    //at the very least, we definitely want to clear the todo item afterwards (perhaps, though, we want to wait until the backend confirms that he saved it [on 3x local/networked/offline machines] before clearing the data TODOreq yes and for all data types in app imo. LOCK GUI UNTIL BACKEND CONFIRMS THAT (regardless of if the newly created item will even be shown/visualized (in this todo list example, if we added it to the end of the list and the end was really far down from where our scroll bars are, we wouldn't be showing/visualizing it (more correctly: we won't have it in memory until we scroll down closer to it)))). --- a lot of this extra decoupling depends on whether or not i even give a shit about the cli/web version... which i've still not yet decided
    //m_NewTodoItemLineEdit->clear();

    //I'm now considering splitting all the tabs into standalone applications and making the backend/core a daemon communicated with via QLocalSocket
    //^con: no longer "flush" user experience. you need to put things right in peoples' face, otherwise they will never see/use it
    //^^still the lazy loaded daemon (gui,cli,web can all load it when they start up (they ideally share one if two front-ends are launched simultaneously, but that leads to solveable synchronicity issues)) isn't a bad idea
}
