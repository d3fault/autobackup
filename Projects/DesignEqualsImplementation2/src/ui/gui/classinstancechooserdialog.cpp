#include "classinstancechooserdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVectorIterator>

#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationusecase.h"

//TODOoptional: instead of a modal dialog, a qlistwidget below the uml class items listing the entries, defaulting to "new top level widget". Just less clicks is all. Also right click -> change instance being used?
ClassInstanceChooserDialog::ClassInstanceChooserDialog(DesignEqualsImplementationClass *classBeingAdded, DesignEqualsImplementationUseCase *useCaseClassIsBeingAddedTo, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_PotentialInstancesListWidget(new QListWidget())
    , m_ClassBeingAdded(classBeingAdded)
    , m_NewTopLevelInstanceChosen(false)
    , m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline(0)
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
        //if(classBeingAdded == currentTopLevelClassInstanceEntryType->m_MyClass)
        //    addInstanceToListWidget(currentTopLevelClassInstanceEntryType);

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
    connect(m_PotentialInstancesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}
bool ClassInstanceChooserDialog::newTopLevelInstanceChosen()
{
    return m_NewTopLevelInstanceChosen;
}
DesignEqualsImplementationClassInstance *ClassInstanceChooserDialog::myInstanceInClassThatHasMe() const
{
    return m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline;
}
void ClassInstanceChooserDialog::addInstanceToListWidget(const QString &absoluteVariableName, DesignEqualsImplementationClassInstance *currentInstance)
{
    QListWidgetItem *potentialClassInstanceListWidgetItem = new QListWidgetItem(absoluteVariableName);

#if 0
    QString fullScopeVariableName;
    QStack<QString> parentStack;
    DesignEqualsImplementationClassInstance *currentInstanceToStackNameOf = currentInstance;
    for(;;)
    {
        parentStack.push(currentInstanceToStackNameOf->VariableName);
        parentStack.push(currentInstanceToStackNameOf->typeString());
        currentInstanceToStackNameOf = currentInstanceToStackNameOf->m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->hasA_Private_Classes_Members().at(currentInstanceToStackNameOf->m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.second);
        if(!currentInstanceToStackNameOf)
            break;
    }
#endif

#if 0
    for(;;)
    {
        parentStack.push(currentInstanceToStackNameOf->VariableName); //TODOreq: the very last "type" should go at the very beginning of the global thing, like this: "Bar *Foo::TopLevel_Foo0::Zed::m_Zed m_Bar".. but idk that's kinda clunky looking already... (and I was trying to say Foo hasA Zed hasA Bar::m_Bar)
        parentStack.push(currentInstanceToStackNameOf->typeString());
        if(!currentInstanceToStackNameOf->m_ParentClassInstanceThatHasMe_OrZeroIfTopLevelObject)
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
    if(m_ClassBeingAdded == currentChildInstance->m_MyClass)
    {
        QString absoluteVariableName = makeAbsoluteVariableNameSpecifierForInstance(currentChildInstance);
        addInstanceToListWidget(absoluteVariableName, currentChildInstance);
    }
    m_AbsoluteVariableNameSpecifierCurrentDepthStack.push(currentChildInstance->typeString());
    m_AbsoluteVariableNameSpecifierCurrentDepthStack.push(currentChildInstance->VariableName);
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentSubChildInstance, currentChildInstance->m_MyClass->hasA_Private_Classes_Members())
    {
        //runningListOfExistingInstances->append(currentSubChildInstance);
        //if(m_ClassBeingAdded == currentSubChildInstance->m_MyClass)
        //    addInstanceToListWidget(currentSubChildInstance);

        //recursion <3
        addAllChildrensChildrenClassInstancesToPassedInList(currentSubChildInstance);
    }
    m_AbsoluteVariableNameSpecifierCurrentDepthStack.pop();
    m_AbsoluteVariableNameSpecifierCurrentDepthStack.pop();
}
QString ClassInstanceChooserDialog::makeAbsoluteVariableNameSpecifierForInstance(DesignEqualsImplementationClassInstance *instanceToFinalizeTheVariableName)
{
    QVectorIterator<QString> scopeStackIterator(m_AbsoluteVariableNameSpecifierCurrentDepthStack);
    QString ret;
    bool firstEntry = true;
    while(scopeStackIterator.hasNext())
    {
        QString currentVariableSpecifierPart = scopeStackIterator.next();
        if(firstEntry)
        {
            ret += currentVariableSpecifierPart;
            firstEntry = false;
            continue;
        }
        ret += "::" + currentVariableSpecifierPart;
    }
    if(!m_AbsoluteVariableNameSpecifierCurrentDepthStack.isEmpty())
        ret += " ---    "; //*<b>*/ +
    ret += instanceToFinalizeTheVariableName->preferredTextualRepresentation(); // + "</b>";
#if 0
    QString ret(instanceToFinalizeTheVariableName->typeString() + " ");
    bool firstEntry = true;
    while(scopeStackIterator.hasNext())
    {
        QString currentVariableSpecifierPart = scopeStackIterator.next();
        if(firstEntry)
        {
            ret += currentVariableSpecifierPart;
            firstEntry = false;
            continue;
        }
        ret += "::" + currentVariableSpecifierPart;
    }
    ret += " " + instanceToFinalizeTheVariableName->VariableName;
#endif
    return ret;
}
void ClassInstanceChooserDialog::handleCurrentRowChanged(int newRow)
{
    if(newRow == 0)
    {
        m_NewTopLevelInstanceChosen = true;
        m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline = 0;
    }
    else
    {
        m_NewTopLevelInstanceChosen = false;
        m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline = qvariant_cast<DesignEqualsImplementationClassInstance*>(m_PotentialInstancesListWidget->item(newRow)->data(Qt::UserRole));
    }
}
