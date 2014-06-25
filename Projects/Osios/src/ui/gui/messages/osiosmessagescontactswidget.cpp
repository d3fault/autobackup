#include "osiosmessagescontactswidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListView>

OsiosMessagesContactsWidget::OsiosMessagesContactsWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *toolsRow = new QHBoxLayout();
    QPushButton *newContactButton = new QPushButton(tr("New Contact"));
    QPushButton *deleteContactButton = new QPushButton(tr("Delete Contact"));
    QPushButton *sendMessageButton = new QPushButton(tr("Send Message")); //TODOreq: double clicking or right clicking
    QListView *contactsListView = new QListView();

    deleteContactButton->setEnabled(false); //TODOreq: enable when contact is clicked, confirm delete (TODOreq: delete, like everything, is undoable (or at least revisitable [via timeline view])
    sendMessageButton->setEnabled(false); //TODOreq: enable when contact is clicked, disable if all contacts deleted

    toolsRow->addWidget(newContactButton);
    toolsRow->addWidget(deleteContactButton);
    toolsRow->addWidget(sendMessageButton);

    myLayout->addLayout(toolsRow);
    myLayout->addWidget(contactsListView, 1);

    setLayout(myLayout);
}
