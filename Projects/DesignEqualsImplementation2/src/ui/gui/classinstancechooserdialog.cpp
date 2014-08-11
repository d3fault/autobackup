#include "classinstancechooserdialog.h"

#include <QRadioButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
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
    , m_NewInstanceRadioButton(new QRadioButton(tr("New Instance")))
    , m_NewInstanceChosen(true)
    , m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance(new QComboBox())
    , m_NewInstanceNameLineEdit(new QLineEdit())
    , m_ExistingInstancesListWidget(new QListWidget())
    , m_ClassBeingAdded(classBeingAdded)
    //TODOinstancing: , m_NewTopLevelInstanceChosen(false)
    //TODOinstancing: , m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline(0)
{
    setWindowTitle(tr("Class Instance Creator/Chooser"));
    if(!classBeingAdded)
    {
        //TODOreq: emit e
        QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
        return;
    }

    QLabel *currentClassBeingAssignedInstanceLabel = new QLabel(tr("Choose instance for: ") + classBeingAdded->ClassName);

    //new instance
    m_NewInstanceRadioButton->setChecked(true);
    QHBoxLayout *newInstanceRow = new QHBoxLayout();
    QFont boldFont;
    boldFont.setBold(true);
    m_NewInstanceRadioButton->setFont(boldFont);
    QLabel *newChildMemberLabel = new QLabel(tr("Child member of:"));
    bool firstClassAdded = true;
    Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeline, useCaseClassIsBeingAddedTo->classLifeLines())
    {
        DesignEqualsImplementationClass *currentClass = currentClassLifeline->designEqualsImplementationClass();
        if(currentClass == classBeingAdded)
            continue; //don't add ourself to ourself (but actually sometimes this is useful so... :-/)
        if(firstClassAdded)
        {
            m_ExistingClassToUseAsParentForNewInstance = currentClass;
            firstClassAdded = false;
        }
        m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance->addItem(currentClass->ClassName, QVariant::fromValue(currentClass));
    }
    m_NewInstanceNameLineEdit->setPlaceholderText(tr("Member Name")); //TODOoptional: m_Bar0, m_Bar1, etc auto-generated-but-editable-obviously
    newInstanceRow->addWidget(newChildMemberLabel);
    newInstanceRow->addWidget(m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance);
    newInstanceRow->addWidget(m_NewInstanceNameLineEdit);

    //existing instances
    QRadioButton *existingInstancesRadioButton = new QRadioButton(tr("Existing Instances"));
    existingInstancesRadioButton->setFont(boldFont);

    Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeline, useCaseClassIsBeingAddedTo->classLifeLines())
    {
        DesignEqualsImplementationClass *currentClass = currentClassLifeline->designEqualsImplementationClass();
        if(currentClass == classBeingAdded)
            continue;

        //TODOreq: two instances of same type (allowed in use case) should only be added once. just keep track of the classes already added and make sure it's not already added, simple

        addAllPrivateHasAMembersThatAreOfAcertainTypeToExistingInstancesListWidget(currentClass, classBeingAdded);
    }

#if 0 // OLD, needs refactoring

    //m_ExistingInstancesListWidget->addItem(QObject::tr("New top-level instance of: ") + classBeingAdded->ClassName); //TODOreq: zero is special index

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
#endif

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Choose Instance"));
    okButton->setDefault(true); //TODOreq: the default/topmost-in-list-widget-what-enter-or-ok-gives-you selected instance should be the last instance of that class type that you ever added to any use case in this project
    okCancelRow->addWidget(cancelButton);
    okCancelRow->addWidget(okButton);

    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->addWidget(currentClassBeingAssignedInstanceLabel);
    myLayout->addWidget(m_NewInstanceRadioButton);
    myLayout->addLayout(newInstanceRow);
    myLayout->addWidget(existingInstancesRadioButton);
    myLayout->addWidget(m_ExistingInstancesListWidget);
    myLayout->addLayout(okCancelRow);
    setLayout(myLayout);

    connect(m_NewInstanceRadioButton, SIGNAL(toggled(bool)), this, SLOT(handleNewInstancesRadioButtonToggled(bool)));
    connect(m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance, SIGNAL(currentIndexChanged(int)), this, SLOT(handleClassesInUseCaseAvailableForUseAsParentOfNewInstanceCurrentIndexChanged(int)));
    connect(m_NewInstanceNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(handleNewInstanceNameLineEditTextChanged(QString)));
    connect(m_ExistingInstancesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(handleExistingInstancesCurrentRowChanged(int)));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_ExistingInstancesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}
bool ClassInstanceChooserDialog::newInstanceChosen()
{
    return m_NewInstanceChosen;
}
DesignEqualsImplementationClass *ClassInstanceChooserDialog::parentClassChosenToGetNewHasAprivateMember() const
{
    return m_ExistingClassToUseAsParentForNewInstance;
}
QString ClassInstanceChooserDialog::nameOfNewPrivateHasAMember() const
{
    return m_NameOfNewPrivateHasAMember;
}
HasA_Private_Classes_Member *ClassInstanceChooserDialog::chosenExistingHasA_Private_Classes_Member() const
{
    return m_ExistingInstanceHasAPrivateClassesMember;
}
void ClassInstanceChooserDialog::addAllPrivateHasAMembersThatAreOfAcertainTypeToExistingInstancesListWidget(DesignEqualsImplementationClass *classToIterate, DesignEqualsImplementationClass *typeOfClassWeAreInterestedInInstancesOf)
{
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateHasAClassMember, classToIterate->hasA_Private_Classes_Members())
    {
        if(currentPrivateHasAClassMember->m_MyClass != typeOfClassWeAreInterestedInInstancesOf)
            continue;

        QListWidgetItem *existingClassInstanceListWidgetItem = new QListWidgetItem(classToIterate->ClassName + "::" + currentPrivateHasAClassMember->VariableName);
        existingClassInstanceListWidgetItem->setData(Qt::UserRole, QVariant::fromValue(currentPrivateHasAClassMember));
        m_ExistingInstancesListWidget->addItem(existingClassInstanceListWidgetItem);
    }
}
void ClassInstanceChooserDialog::handleNewInstancesRadioButtonToggled(bool checked)
{
    m_NewInstanceChosen = checked;
}
#if 0 //TODOinstancing
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
    m_ExistingInstancesListWidget->addItem(potentialClassInstanceListWidgetItem);
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
#endif
void ClassInstanceChooserDialog::handleClassesInUseCaseAvailableForUseAsParentOfNewInstanceCurrentIndexChanged(int newIndex)
{
    m_ExistingClassToUseAsParentForNewInstance = qvariant_cast<DesignEqualsImplementationClass*>(m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance->itemData(newIndex));
    //tryValidatingDialog();
}
void ClassInstanceChooserDialog::handleNewInstanceNameLineEditTextChanged(const QString &newText)
{
    m_NameOfNewPrivateHasAMember = newText;
}
void ClassInstanceChooserDialog::handleExistingInstancesCurrentRowChanged(int newRow)
{
    m_ExistingInstanceHasAPrivateClassesMember = qvariant_cast<HasA_Private_Classes_Member*>(m_ExistingInstancesListWidget->item(newRow)->data(Qt::UserRole));
}
