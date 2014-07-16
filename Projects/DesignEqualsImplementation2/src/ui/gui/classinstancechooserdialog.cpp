#include "classinstancechooserdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStack>

#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationusecase.h"

//
ClassInstanceChooserDialog::ClassInstanceChooserDialog(DesignEqualsImplementationClass *classBeingAdded, DesignEqualsImplementationUseCase *useCaseClassIsBeingAddedTo, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_PotentialInstancesListWidget(new QListWidget())
    , m_ClassBeingAdded(classBeingAdded)
    , m_NewTopLevelInstanceChosen(false)
    , m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject(0)
{
    setWindowTitle(tr("Class Instance Chooser"));
    if(!classBeingAdded)
    {
        //TODOreq: emit e
        QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
        return;
    }

    //new instances
    m_PotentialInstancesListWidget->addItem(QObject::tr("New top-level instance of: ") + classBeingAdded->ClassName); //TODOreq: zero is special index

    //QList<DesignEqualsImplementationClassInstance*> potentialExistingInstances;
    Q_FOREACH(DesignEqualsImplementationClassInstance* currentTopLevelClassInstanceEntryType, useCaseClassIsBeingAddedTo->designEqualsImplementationProject()->topLevelClassInstances()) //Top level objects + heirarchies?
    {
        //potentialExistencesInParents.append(currentTopLevelClassInstanceEntryType);
        if(classBeingAdded == currentTopLevelClassInstanceEntryType->m_MyClass)
            addInstanceToListWidget(currentTopLevelClassInstanceEntryType);

#if 0 //only the children of the top level objects will have it != 0 (TODOreq: ensure "0" validity stays synchronized... somewhere (the setters?))
        if(currentclassInstanceEntryType.first == 0)
        {
            //top level object
        }
        else
        {
            //has parent (as of writing, means parent hasA child)

        }
#endif
        //recursively add all children
        addAllChildrensChildrenClassInstancesToPassedInList(currentTopLevelClassInstanceEntryType);
    }

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(tr("Choose Instance"));
    okButton->setDefault(true); //TODOreq: the default/topmost-in-list-widget-what-enter-or-ok-gives-you selected instance should be the last instance of that class type that you ever added to any use case in this project
    okCancelRow->addWidget(okButton);

    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->addWidget(m_PotentialInstancesListWidget);
    myLayout->addLayout(okCancelRow);
    setLayout(myLayout);

    connect(m_PotentialInstancesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(handleCurrentRowChanged(int)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}
bool ClassInstanceChooserDialog::newTopLevelInstanceChosen()
{
    return m_NewTopLevelInstanceChosen;
}
DesignEqualsImplementationClassInstance *ClassInstanceChooserDialog::myInstanceInClassThatHasMe() const
{
    return m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject;
}
void ClassInstanceChooserDialog::addInstanceToListWidget(DesignEqualsImplementationClassInstance *currentInstance)
{
    QListWidgetItem *potentialClassInstanceListWidgetItem = new QListWidgetItem(currentInstance->VariableName /*TODOreq: full recursive scope specification extraction so user can see it in the dialog*/);
#if 0 //attempt semi-fail (taking a simpler direction)
    QString fullScopeVariableName;
    QStack<QString> parentStack;
    DesignEqualsImplementationClassInstance *currentParent = currentInstance;
    for(;;)
    {
        parentStack.push(currentParent->VariableName); //TODOreq: the very last "type" should go at the very beginning of the global thing, like this: "Bar *Foo::TopLevel_Foo0::Zed::m_Zed m_Bar".. but idk that's kinda clunky looking already... (and I was trying to say Foo hasA Zed hasA Bar::m_Bar)
        currentParent = currentParent->m_ParentClassThatHasMe_OrZeroIfTopLevelObject;
        if(!currentParent)
            break;
    }
    while(parentStack.isEmpty())
    {
        fullScopeVariableName += parentStack.pop() + "::";
    }
#endif

    potentialClassInstanceListWidgetItem->setData(Qt::UserRole, QVariant::fromValue(currentInstance));
    m_PotentialInstancesListWidget->addItem(potentialClassInstanceListWidgetItem);
}
void ClassInstanceChooserDialog::addAllChildrensChildrenClassInstancesToPassedInList(DesignEqualsImplementationClassInstance *currentChildInstance/*, QList<DesignEqualsImplementationClassInstance*> *runningListOfExistingInstances*/)
{
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentSubChildInstance, currentChildInstance->m_MyClass->HasA_Private_Classes_Members)
    {
        //runningListOfExistingInstances->append(currentSubChildInstance);
        if(m_ClassBeingAdded == currentSubChildInstance->m_MyClass)
            addInstanceToListWidget(currentSubChildInstance);

        //recursion <3
        addAllChildrensChildrenClassInstancesToPassedInList(currentSubChildInstance);
    }
}
void ClassInstanceChooserDialog::handleCurrentRowChanged(int newRow)
{
    if(newRow == 0)
    {
        m_NewTopLevelInstanceChosen = true;
        m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject = 0;
    }
    else
    {
        m_NewTopLevelInstanceChosen = false;
        m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject = qvariant_cast<DesignEqualsImplementationClassInstance*>(m_PotentialInstancesListWidget->item(newRow)->data(Qt::UserRole));
    }
}
