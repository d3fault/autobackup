#include "classinstancechooserdialog.h"

#include <QRadioButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVectorIterator>

#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationusecase.h"
#include "../../designequalsimplementationclasslifeline.h"

//TODOoptional: instead of a modal dialog, a qlistwidget below the uml class items listing the entries, defaulting to "new top level widget". Just less clicks is all. Also right click -> change instance being used?
//TODOoptional: radio mode grays out opposite mode
//TODOreq: introducing qgoupbox made the radio buttons in different groups (and so not mutually exclusive), fix that
//TODOreq: when instance choser is opened and we already have an instance, the "existing instance" radio box should be pre-selected, as well as the actual instance in the list highlighted
//TODOreq: "root use case class lifeline" radio option (name not applicable). radio option not shown if use case already has root class lifeline
ClassInstanceChooserDialog::ClassInstanceChooserDialog(DesignEqualsImplementationClassLifeLine *classLifelineForWhichAnInstanceIsBeingChosen, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_NewInstanceRadioButton(new QRadioButton(tr("New Instance")))
    , m_NewInstanceChosen(true)
    , m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance(new QComboBox())
    , m_NewInstanceNameLineEdit(new QLineEdit())
    , m_ExistingInstancesListWidget(new QListWidget())
    , m_ClassLifelineForWhichAnInstanceIsBeingChosen(classLifelineForWhichAnInstanceIsBeingChosen)
    , m_UserIsTypingInCustomVariableNameSoDontSuggestAutoName(false)
    //TODOinstancing: , m_NewTopLevelInstanceChosen(false)
    //TODOinstancing: , m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline(0)
{
    setWindowTitle(tr("Class Instance Creator/Chooser"));
    if(!classLifelineForWhichAnInstanceIsBeingChosen)
    {
        //TODOreq: emit e
        QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
        return;
    }

    QLabel *currentClassBeingAssignedInstanceLabel = new QLabel(tr("Choose instance for object of type: ") + classLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass()->Name);

    //new instance
    QGroupBox *newInstanceGroupBox = new QGroupBox();
    QVBoxLayout *newInstanceLayout = new QVBoxLayout();
    m_NewInstanceRadioButton->setChecked(true);
    QHBoxLayout *newInstanceRow = new QHBoxLayout();
    QFont boldFont;
    boldFont.setBold(true);
    m_NewInstanceRadioButton->setFont(boldFont);
    QLabel *newChildMemberLabel = new QLabel(tr("Child member of:"));
    bool firstClassAdded = true;
    Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeline, classLifelineForWhichAnInstanceIsBeingChosen->parentUseCase()->classLifeLines())
    {
        DesignEqualsImplementationClass *currentClass = currentClassLifeline->designEqualsImplementationClass();
        if(currentClass == classLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass())
            continue; //don't add ourself to ourself (but actually sometimes this is useful so... :-/)
        if(firstClassAdded)
        {
            m_ExistingClassToUseAsParentForNewInstance = currentClass;
            firstClassAdded = false;
        }
        m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance->addItem(currentClass->Name, QVariant::fromValue(currentClass));
    }
    m_NewInstanceNameLineEdit->setPlaceholderText(tr("Member Name")); //TODOoptional: m_Bar0, m_Bar1, etc auto-generated-but-editable-obviously
    newInstanceRow->addWidget(newChildMemberLabel);
    newInstanceRow->addWidget(m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance);
    newInstanceRow->addWidget(m_NewInstanceNameLineEdit);

    //existing instances
    QGroupBox *existingInstanceGroupBox = new QGroupBox();
    QVBoxLayout *existingInstancesLayout = new QVBoxLayout();
    QRadioButton *existingInstancesRadioButton = new QRadioButton(tr("Existing Instances"));
    existingInstancesRadioButton->setFont(boldFont);

    bool atLeastOneOtherClassLifeline = false;
    Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeline, classLifelineForWhichAnInstanceIsBeingChosen->parentUseCase()->classLifeLines())
    {
        DesignEqualsImplementationClass *currentClass = currentClassLifeline->designEqualsImplementationClass();
        if(currentClass == classLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass())
            continue;

        if(!atLeastOneOtherClassLifeline)
        {
            atLeastOneOtherClassLifeline = true;

            //atLeastOne also used hackily as a "firstAdded" detector for auto variable name selection
            m_NewInstanceNameLineEdit->setText(currentClass->autoNameForNewChildMemberOfType(classLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass()->Name)); //when new drop-down is chosen, a new auto-name for the chosen class is generated, UNLESS user has manually typed anything ever (during the lifetime of this modal dialog). TODOoptional: set focus to the line edit and select the "0" portion of it (the auto-generated portion), allowing them to type immediately and have that portion of it replaced. ex: m_Foo0 and the zero is highlighted ready for replacing with something more descriptive. this same feature could appear in many different places throughout app. In fact, that portion of text could be highlighted right when the dialog is first shown
        }

        //TODOreq: two instances of same type (allowed in use case) should only be added once. just keep track of the classes already added and make sure it's not already added, simple

        addAllPrivateHasAMembersThatAreOfAcertainTypeToExistingInstancesListWidget(currentClass, classLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass());
    }

#if 0 // OLD, needs refactoring

    //m_ExistingInstancesListWidget->addItem(QObject::tr("New top-level instance of: ") + classBeingAdded->Name); //TODOreq: zero is special index

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

    newInstanceLayout->addWidget(m_NewInstanceRadioButton);
    newInstanceLayout->addLayout(newInstanceRow);
    newInstanceGroupBox->setLayout(newInstanceLayout);

    existingInstancesLayout->addWidget(existingInstancesRadioButton);
    existingInstancesLayout->addWidget(m_ExistingInstancesListWidget);
    existingInstanceGroupBox->setLayout(existingInstancesLayout);

    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->addWidget(currentClassBeingAssignedInstanceLabel);
    myLayout->addWidget(newInstanceGroupBox);
    myLayout->addWidget(existingInstanceGroupBox);
    myLayout->addLayout(okCancelRow);
    setLayout(myLayout);

    connect(m_NewInstanceRadioButton, SIGNAL(toggled(bool)), this, SLOT(handleNewInstancesRadioButtonToggled(bool)));
    connect(m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance, SIGNAL(currentIndexChanged(int)), this, SLOT(handleClassesInUseCaseAvailableForUseAsParentOfNewInstanceCurrentIndexChanged(int)));
    connect(m_NewInstanceNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(handleNewInstanceNameLineEditTextChanged(QString)));
    connect(m_ExistingInstancesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(handleExistingInstancesCurrentRowChanged(int)));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_ExistingInstancesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    connect(this, SIGNAL(accepted()), this, SLOT(handleDialogAccepted()));

    connect(this, SIGNAL(createNewNonFunctionMemberAndAssignItAsClassLifelineInstanceRequested(DesignEqualsImplementationClass*,DesignEqualsImplementationClass*,QString)), classLifelineForWhichAnInstanceIsBeingChosen, SLOT(createNewNonFunctionMemberAndAssignItAsClassLifelineInstance(DesignEqualsImplementationClass*,DesignEqualsImplementationClass*,QString)));
    connect(this, SIGNAL(assignNonFunctionMemberAsClassLifelineInstanceRequested(NonFunctionMember*)), classLifelineForWhichAnInstanceIsBeingChosen, SLOT(assignNonFunctionMemberAsClassLifelineInstance(NonFunctionMember*)));

    if(!atLeastOneOtherClassLifeline)
    {
        okButton->setDisabled(true);
    }
}
bool ClassInstanceChooserDialog::newInstanceChosen()
{
    return m_NewInstanceChosen;
}
DesignEqualsImplementationClass *ClassInstanceChooserDialog::parentClassChosenToGetNewHasAprivateMember() const
{
    return m_ExistingClassToUseAsParentForNewInstance;
}
void ClassInstanceChooserDialog::addAllPrivateHasAMembersThatAreOfAcertainTypeToExistingInstancesListWidget(DesignEqualsImplementationClass *classToIterate, DesignEqualsImplementationClass *typeOfClassWeAreInterestedInInstancesOf)
{
    //TODOreq: if the existing instance is already making an appearance in this use case (and it is not us), then i'm pretty sure it shouldn't be shown in the existing instances list. it depends on whether or not there can be two class lifeline instances that are the same, which i'm pretty sure is not allowed
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, classToIterate->nonFunctionMembers())
    {
        DesignEqualsImplementationClass *aClassThatIsANonFunctionMemberMaybe = qobject_cast<DesignEqualsImplementationClass*>(currentNonFunctionMember->type);
        if(!aClassThatIsANonFunctionMemberMaybe)
            continue; //only DesignEqualsImplementationClasses can be used as class lifeline instances. the other types don't have signals/slots!

        if(aClassThatIsANonFunctionMemberMaybe != typeOfClassWeAreInterestedInInstancesOf)
            continue;

        QListWidgetItem *existingClassInstanceListWidgetItem = new QListWidgetItem(classToIterate->Name + "::" + currentNonFunctionMember->VariableName);
        existingClassInstanceListWidgetItem->setData(Qt::UserRole, QVariant::fromValue(currentNonFunctionMember));
        m_ExistingInstancesListWidget->addItem(existingClassInstanceListWidgetItem);
    }
#if 0
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateHasAClassMember, classToIterate->hasA_Private_Classes_Members())
    {
        if(currentPrivateHasAClassMember->m_MyClass != typeOfClassWeAreInterestedInInstancesOf)
            continue;

        QListWidgetItem *existingClassInstanceListWidgetItem = new QListWidgetItem(classToIterate->Name + "::" + currentPrivateHasAClassMember->VariableName);
        existingClassInstanceListWidgetItem->setData(Qt::UserRole, QVariant::fromValue(currentPrivateHasAClassMember));
        m_ExistingInstancesListWidget->addItem(existingClassInstanceListWidgetItem);
    }
#endif
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
    if(!m_UserIsTypingInCustomVariableNameSoDontSuggestAutoName || m_NewInstanceNameLineEdit->text().trimmed().isEmpty())
    {
        m_NewInstanceNameLineEdit->setText(m_ExistingClassToUseAsParentForNewInstance->autoNameForNewChildMemberOfType(m_ClassLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass()->Name));
    }
    //tryValidatingDialog();
}
void ClassInstanceChooserDialog::handleNewInstanceNameLineEditTextChanged(const QString &newText)
{
    m_UserIsTypingInCustomVariableNameSoDontSuggestAutoName = true;
    m_NameOfNewNonFunctionMember = newText;
}
void ClassInstanceChooserDialog::handleExistingInstancesCurrentRowChanged(int newRow)
{
    m_ExistingInstanceNonFunctionMember = qvariant_cast<NonFunctionMember*>(m_ExistingInstancesListWidget->item(newRow)->data(Qt::UserRole));
}
void ClassInstanceChooserDialog::handleDialogAccepted()
{
    if(newInstanceChosen())
    {
        emit createNewNonFunctionMemberAndAssignItAsClassLifelineInstanceRequested(parentClassChosenToGetNewHasAprivateMember(), m_ClassLifelineForWhichAnInstanceIsBeingChosen->designEqualsImplementationClass(), m_NameOfNewNonFunctionMember);
    }
    else
    {
        emit assignNonFunctionMemberAsClassLifelineInstanceRequested(m_ExistingInstanceNonFunctionMember);
    }
}
